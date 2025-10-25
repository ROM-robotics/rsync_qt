#include "ros_bridge_client.hpp"
#include "rom_structures.h" // for ROM_COLOR_* macros
#include "rom_map.hpp"
#include <QDateTime>
#include <QDebug>
#include <cmath>
#include <QMetaType>

RosBridgeClient::RosBridgeClient(const QString &robot_ns, const QString &host, quint16 port, QObject *parent)
    : QObject(parent), m_robotNamespace(robot_ns), m_host(host), m_port(port) {
    // Ensure custom types used in signals are registered for queued connections
    qRegisterMetaType<RomTF>("RomTF");
    qRegisterMetaType<TransformStamped>("TransformStamped");

    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[      RosBridgeClient::RosBridgeClient      ]%2")
                              .arg(ROM_COLOR_GREEN).arg(ROM_COLOR_RESET);
    #endif
    connect(&m_socket, &QWebSocket::connected, this, &RosBridgeClient::onSocketConnected);
    connect(&m_socket, &QWebSocket::disconnected, this, &RosBridgeClient::onSocketDisconnected);
    connect(&m_socket, &QWebSocket::textMessageReceived, this, &RosBridgeClient::onTextMessageReceived);
    connect(&m_socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred), this, &RosBridgeClient::onSocketError);

    m_reconnectTimer.setInterval(3000);
    m_reconnectTimer.setSingleShot(true);
    connect(&m_reconnectTimer, &QTimer::timeout, this, &RosBridgeClient::ensureReconnect);

    // 20 Hz cmd_vel publisher timer
    m_cmdVelPubTimer.setInterval(50); // 20 Hz
    m_cmdVelPubTimer.setTimerType(Qt::CoarseTimer);
    connect(&m_cmdVelPubTimer, &QTimer::timeout, this, &RosBridgeClient::onCmdVelPubTimer);
    m_cmdVelPubTimer.start();
}

void RosBridgeClient::connectToServer() 
{
    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[      RosBridgeClient::connectToServer      ]%2")
                              .arg(ROM_COLOR_GREEN).arg(ROM_COLOR_RESET);
    #endif

    if (isConnected()) return;

    const QUrl url(QStringLiteral("ws://%1:%2").arg(m_host).arg(m_port));
    m_socket.open(url);
}

void RosBridgeClient::disconnectFromServer() 
{
    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[    RosBridgeClient::disconnectFromServer   ]%2")
                              .arg(ROM_COLOR_GREEN).arg(ROM_COLOR_RESET);
    #endif

    m_reconnectTimer.stop();
    m_cmdVelPubTimer.stop();
    if (isConnected()) 
    {
        m_socket.close();
    }
}

void RosBridgeClient::sendJson(const QJsonObject &obj) 
{
    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[         RosBridgeClient::sendJson          ]%2")
                              .arg(ROM_COLOR_GREEN).arg(ROM_COLOR_RESET);
    #endif

    QJsonDocument doc(obj);
    m_socket.sendTextMessage(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

void RosBridgeClient::ensureReconnect() 
{
    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[      RosBridgeClient::ensureReconnect      ]%2")
                              .arg(ROM_COLOR_GREEN).arg(ROM_COLOR_RESET);
    #endif

    if ((m_cmdVelSubscribed || 
        m_mapSubscribed || 
        m_tfSubscribed || 
        m_frontLaserSubscribed || 
        m_odomSubscribed || 
        m_controllerOdomSubscribed || m_mapBfpSubscribed) && !isConnected()) {
        connectToServer();
    }
}


void RosBridgeClient::onSocketConnected() 
{
    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[     RosBridgeClient::onSocketConnected     ]%2")
                              .arg(ROM_COLOR_GREEN).arg(ROM_COLOR_RESET);
    #endif

    emit connected();
    if (m_cmdVelSubscribed) 
    {
        subscribeCmdVel();
    }
    if (m_mapSubscribed) 
    {
        subscribeMap();
    }
    if (m_tfSubscribed) 
    {
        subscribeTF();
    }
    if (m_frontLaserSubscribed) 
    {
        subscribeFrontLaserScan();
    }
    if (m_odomSubscribed) 
    {
        subscribeOdom();
    }
    if (m_controllerOdomSubscribed) 
    {
        subscribeControllerOdom();
    }
    // if (m_rearLaserSubscribed) {
    //     subscribeRearLaserScan();
    // }
    if(m_mapBfpSubscribed)
    {
        subscribeMapBfp();
    }
}

void RosBridgeClient::onSocketDisconnected() 
{
    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[    RosBridgeClient::onSocketDisconnected   ]%2")
                              .arg(ROM_COLOR_GREEN).arg(ROM_COLOR_RESET);
    #endif

    emit disconnected();

    if (m_cmdVelSubscribed ||
        m_mapSubscribed ||
        m_tfSubscribed ||
        m_frontLaserSubscribed ||
        m_odomSubscribed ||
        m_controllerOdomSubscribed || m_mapBfpSubscribed) 
    {
        m_reconnectTimer.start();
    }
    // if (m_rearLaserSubscribed) {
    //     m_reconnectTimer.start();
    // }
}

void RosBridgeClient::onSocketError(QAbstractSocket::SocketError) 
{
    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[       RosBridgeClient::onSocketError       ]%2")
                              .arg(ROM_COLOR_GREEN).arg(ROM_COLOR_RESET);
    #endif

    emit errorOccurred(m_socket.errorString());
    if (!isConnected()) 
    {
        m_reconnectTimer.start();
    }
}

void RosBridgeClient::onTextMessageReceived(const QString &msg) 
{

    QJsonParseError err{};
    QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8(), &err);

    if (err.error != QJsonParseError::NoError) 
    {
        #ifdef ROM_DEBUG
            qWarning() << "RosBridgeClient JSON parse error:" << err.errorString();
        #endif

        return;
    }

    if (!doc.isObject()) return;
    QJsonObject obj = doc.object();

    const QString op = obj.value("op").toString();
    if (op == "publish") 
    {
        const QString topic = obj.value("topic").toString();
        if (topic == cmd_vel_topic_name) 
        {
            QJsonObject m = obj.value("msg").toObject();
            QJsonObject lin = m.value("linear").toObject();
            QJsonObject ang = m.value("angular").toObject();

            TwistData data;

            data.linear_x = lin.value("x").toDouble();
            data.linear_y = lin.value("y").toDouble();
            data.linear_z = lin.value("z").toDouble();
            data.angular_x = ang.value("x").toDouble();
            data.angular_y = ang.value("y").toDouble();
            data.angular_z = ang.value("z").toDouble();
            qint64 tsUsec = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch() * 1000;
            emit twistReceived(data, tsUsec);
        } 
        else if (topic == map_topic_name) 
        {
            // Parse nav_msgs/OccupancyGrid
            // Expected structure: msg: { header:{stamp:{sec,nsec}}, info:{width,height,resolution,origin:{position:{x,y}}}, data:[...] }
            QJsonObject m = obj.value("msg").toObject();
            QJsonObject info = m.value("info").toObject();
            int width = info.value("width").toInt();
            int height = info.value("height").toInt();
            double resolution = info.value("resolution").toDouble();
            QJsonObject origin = info.value("origin").toObject();
            QJsonObject origin_pos = origin.value("position").toObject();
            double origin_x = origin_pos.value("x").toDouble();
            double origin_y = origin_pos.value("y").toDouble();

            // timestamp
            Time ts; // default
            QJsonObject header = m.value("header").toObject();
            QJsonObject stamp = header.value("stamp").toObject();
            if (!stamp.isEmpty()) {
                // Assuming Time has constructor Time(sec, nsec) or setters; adapt if different
                if (stamp.contains("sec") && stamp.contains("nsec")) {
                    // We'll try to use reflection of Time API; if unavailable adjust accordingly.
                    // Placeholder: ts = Time(stamp.value("sec").toInt(), stamp.value("nsec").toInt());
                }
            }

            std::vector<int8_t> data_vec;
            QJsonArray dataArr = m.value("data").toArray();
            data_vec.reserve(dataArr.size());
            for (const QJsonValue &v : dataArr) {
                int val = v.toInt();
                if (val > 127) val -= 256; // convert to signed int8 range if necessary
                data_vec.push_back(static_cast<int8_t>(val));
            }

            RomMap map(width, height, 
                static_cast<float>(resolution), 
                static_cast<float>(origin_x), 
                static_cast<float>(origin_y), 
                data_vec, ts);
                
            emit mapReceived(map);
            //emit mapReceived(std::move(map));

            #ifdef ROM_DEBUG
                qDebug().noquote() << QString("%1[   RosBridgeClient::onTextMessageReceived   ] : get map topic ok %2")
                              .arg(ROM_COLOR_GREEN).arg(ROM_COLOR_RESET);
            #endif
        }
        else if (topic == tf_topic_name)
        {
            // Parse tf2_msgs/TFMessage published via rosbridge
            QJsonObject msg = obj.value("msg").toObject();
            QJsonArray transforms = msg.value("transforms").toArray();
            // qDebug() << "Receive Transforms: " << transforms;
            RomTF tfData;

            for (const QJsonValue &tv : transforms) {
                QJsonObject t = tv.toObject();

                QString parent = t.value("header").toObject().value("frame_id").toString();
                QString child  = t.value("child_frame_id").toString();

                QJsonObject stamp = t.value("header").toObject().value("stamp").toObject();
                HackTime rom_time;
                rom_time.sec = stamp.value("sec").toInt();
                rom_time.nanosec = stamp.contains("nanosec")
                                    ? stamp.value("nanosec").toInt()
                                    : stamp.value("nsec").toInt();

                QJsonObject transform = t.value("transform").toObject();
                QJsonObject trans = transform.value("translation").toObject();
                QJsonObject rot   = transform.value("rotation").toObject();

                TransformStamped ts;
                ts.translation = QVector3D(trans.value("x").toDouble(),
                                        trans.value("y").toDouble(),
                                        trans.value("z").toDouble());
                ts.rotation = QQuaternion(rot.value("w").toDouble(),
                                        rot.value("x").toDouble(),
                                        rot.value("y").toDouble(),
                                        rot.value("z").toDouble());
                ts.rom_timestamp = rom_time;

                // Match and assign to appropriate field
                if (parent == "map" && child == "odom") 
                {
                    global_tfData.map_odom = ts;
                    emit_tf = false;
                } 
                else if (parent == "odom" && child == "base_footprint") 
                {
                    tfData.odom_base_footprint = ts;
                    tfData.map_odom = global_tfData.map_odom;
                    emit_tf = true;
                } 
            }

            if(emit_tf)
            {
                qint64 tsUsec = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch() * 1000;
                emit tfReceived(tfData, tsUsec);
            }
            
        }
        else if (topic == front_laser_scan_topic_name || topic == rear_laser_scan_topic_name)
        {
            // Parse sensor_msgs/LaserScan
            // Expected structure: msg: { header:{frame_id, stamp{sec,nanosec}}, angle_min, angle_max, angle_increment, range_min, range_max, ranges:[...], intensities:[...] }
            QJsonObject m = obj.value("msg").toObject();
            RomLaser laser;
            QJsonObject header = m.value("header").toObject();
            laser.m_frameId = header.value("frame_id").toString();
            QJsonObject stamp = header.value("stamp").toObject();
            laser.rom_timestamp.sec = stamp.value("sec").toInt();
            laser.rom_timestamp.nanosec = stamp.contains("nanosec") ? stamp.value("nanosec").toInt() : stamp.value("nsec").toInt();

            laser.m_angleMin = static_cast<float>(m.value("angle_min").toDouble());
            laser.m_angleMax = static_cast<float>(m.value("angle_max").toDouble());
            laser.m_angleIncrement = static_cast<float>(m.value("angle_increment").toDouble());
            laser.m_rangeMin = static_cast<float>(m.value("range_min").toDouble());
            laser.m_rangeMax = static_cast<float>(m.value("range_max").toDouble());

            QJsonArray ranges = m.value("ranges").toArray();
            laser.m_ranges.reserve(ranges.size());
            for (const auto &v : ranges) laser.m_ranges.push_back(static_cast<float>(v.toDouble()));

            QJsonArray intensities = m.value("intensities").toArray();
            laser.m_intensities.reserve(intensities.size());
            for (const auto &v : intensities) laser.m_intensities.push_back(static_cast<float>(v.toDouble()));

            qint64 tsUsec = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch() * 1000;
            if (topic == front_laser_scan_topic_name) 
            {
                emit laserFrontReceived(laser, tsUsec);
            } else 
            {
                //emit laserRearReceived(laser, tsUsec);
            }
        }
        else if (topic == odom_topic_name || topic == controller_odom_topic_name)
        {
            // Parse nav_msgs/Odometry
            QJsonObject m = obj.value("msg").toObject();
            RomOdom od;

            // Header
            QJsonObject header = m.value("header").toObject();
            od.m_frameId = header.value("frame_id").toString();
            QJsonObject stamp = header.value("stamp").toObject();
            od.rom_timestamp.sec = stamp.value("sec").toInt();
            od.rom_timestamp.nanosec = stamp.contains("nanosec") ? stamp.value("nanosec").toInt() : stamp.value("nsec").toInt();

            // Child frame id
            od.m_childFrameId = m.value("child_frame_id").toString();

            // Pose
            QJsonObject pose = m.value("pose").toObject();
            QJsonObject pose_pose = pose.value("pose").toObject();
            QJsonObject pos = pose_pose.value("position").toObject();
            QJsonObject ori = pose_pose.value("orientation").toObject();
            od.m_pose.position = QVector3D(pos.value("x").toDouble(), pos.value("y").toDouble(), pos.value("z").toDouble());
            od.m_pose.orientation = QQuaternion(ori.value("w").toDouble(), ori.value("x").toDouble(), ori.value("y").toDouble(), ori.value("z").toDouble());
            // covariance array (36 doubles)
            for (int i = 0; i < 36; ++i) od.m_pose.covariance[i] = 0.0;
            QJsonArray pose_cov = pose.value("covariance").toArray();
            for (int i = 0; i < pose_cov.size() && i < 36; ++i) {
                od.m_pose.covariance[i] = pose_cov.at(i).toDouble();
            }

            // Twist
            QJsonObject twist = m.value("twist").toObject();
            QJsonObject twist_twist = twist.value("twist").toObject();
            QJsonObject lin = twist_twist.value("linear").toObject();
            QJsonObject ang = twist_twist.value("angular").toObject();
            od.m_twist.linear = QVector3D(lin.value("x").toDouble(), lin.value("y").toDouble(), lin.value("z").toDouble());
            od.m_twist.angular = QVector3D(ang.value("x").toDouble(), ang.value("y").toDouble(), ang.value("z").toDouble());
            for (int i = 0; i < 36; ++i) od.m_twist.covariance[i] = 0.0;
            QJsonArray twist_cov = twist.value("covariance").toArray();
            for (int i = 0; i < twist_cov.size() && i < 36; ++i) {
                od.m_twist.covariance[i] = twist_cov.at(i).toDouble();
            }

            qint64 tsUsec = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch() * 1000;
            if (topic == odom_topic_name) {
                emit odomReceived(od, tsUsec);
            } else {
                emit controllerOdomReceived(od, tsUsec);
            }
        }
        else if (topic == map_bfp_topic_name) 
        {
            QJsonObject m = obj.value("msg").toObject();
            double x = m.value("x").toDouble();
            double y = m.value("y").toDouble();
            double theta = m.value("theta").toDouble();

            qint64 tsUsec = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch() * 1000;
            RomPose2D rom_pose2d = {x, y, theta};
            emit mapBfpReceived(rom_pose2d, tsUsec);
        } 

    }
    

    else if (op == "service_response") 
    {
        // Handle response to GetMap service
        const QString id = obj.value("id").toString();

        if (!m_mapRequestId.isEmpty() && id == m_mapRequestId) 
        {
            // ROS 2 GetMap returns { result: { map: OccupancyGrid } }
            QJsonObject result = obj.value("result").toObject();
            QJsonObject m = result.value("map").toObject();
            if (!m.isEmpty()) {
                QJsonObject info = m.value("info").toObject();
                int width = info.value("width").toInt();
                int height = info.value("height").toInt();
                double resolution = info.value("resolution").toDouble();
                QJsonObject origin = info.value("origin").toObject();
                QJsonObject origin_pos = origin.value("position").toObject();
                double origin_x = origin_pos.value("x").toDouble();
                double origin_y = origin_pos.value("y").toDouble();

                // timestamp from header if provided
                Time ts;
                QJsonObject header = m.value("header").toObject();
                QJsonObject stamp = header.value("stamp").toObject();
                if (!stamp.isEmpty()) {
                    int sec = stamp.value("sec").toInt();
                    int nsec = stamp.contains("nanosec") ? stamp.value("nanosec").toInt()
                                                           : stamp.value("nsec").toInt();
                    ts = Time(sec, nsec);
                }

                std::vector<int8_t> data_vec;
                QJsonArray dataArr = m.value("data").toArray();
                data_vec.reserve(dataArr.size());
                for (const QJsonValue &v : dataArr) {
                    int val = v.toInt();
                    if (val > 127) val -= 256;
                    data_vec.push_back(static_cast<int8_t>(val));
                }

                RomMap map(width, height, static_cast<float>(resolution), static_cast<float>(origin_x), static_cast<float>(origin_y), data_vec, ts);
                //emit mapReceived(map);
                //qDebug() << "Map received via service response.";
            }
            // clear request id so we don't re-handle other responses
            m_mapRequestId.clear();

            // server response မရဘူးလို့ ယူဆထားတယ်။
        }
        
        else if (!m_hsRequestId.isEmpty() && id == m_hsRequestId) 
        {
            qDebug() << "HS response received." << obj;

            // rosbridge call_service returns payload under 'values'
            const QJsonObject values = obj.value("values").toObject();
            const int total_maps = values.value("total_maps").toInt(-1);
            const int status = values.value("status").toInt(-1);
            const QString robot_namespace = values.value("robot_namespace").toString();
            const QJsonArray map_names_arr = values.value("map_names").toArray();
            QStringList map_names;
            map_names.reserve(map_names_arr.size());
            for (const QJsonValue &v : map_names_arr) map_names.push_back(v.toString());

            //emit hsReceived(total_maps, map_names, robot_namespace);
            qDebug().noquote() << QString("%1[          RosBridgeClient::requestHSOnce response       ]%2")
                                   .arg(ROM_COLOR_GREEN)
                                   .arg(QString("Total maps: %1, Map names: [%2], Robot namespace: %3, Status: %4")
                                        .arg(total_maps)
                                        .arg(map_names.join(", "))
                                        .arg(robot_namespace)
                                        .arg(status))
                                   .arg(ROM_COLOR_RESET);

            m_hsRequestId.clear();
        }

        else if (!navi_mode_request_id.isEmpty() && id == navi_mode_request_id)
        {
            #ifdef ROM_THIRD_STAGE_DEBUG
            qDebug() << "Navi response received." << obj;
            #endif

            const QJsonObject values = obj.value("values").toObject();

            const int status = values.value("status").toInt(-2);

            if(status == -2)
            {
                // parsed successfully
                qWarning() << "Failed to parse status in navigation mode response.";
                emit navigation_mode_response_received(status);
            }
            else
            {
                emit navigation_mode_response_received(status); // values to int parse error
            }

            #ifdef ROM_THIRD_STAGE_DEBUG
            qDebug().noquote() << QString("%1[          RosBridgeClient::requestNavigationMode response       ] : %2 %3 ")
                                .arg(ROM_COLOR_GREEN)
                                .arg(status)
                                .arg(ROM_COLOR_RESET);
            #endif

            navi_mode_request_id.clear();
        }

        else if (!mapping_mode_request_id.isEmpty() && id == mapping_mode_request_id) 
        {
            #ifdef ROM_THIRD_STAGE_DEBUG
            qDebug() << "mapping response received." << obj;
            #endif

            const QJsonObject values = obj.value("values").toObject();

            //const int total_maps = values.value("total_maps").toInt(-1);
            const int status = values.value("status").toInt(-2);

            if(status == -2)
            {
                qWarning() << "Failed to parse status in mapping mode response.";
                emit mapping_mode_response_received(status);
            }
            else
            {
                emit mapping_mode_response_received(status); // values to int parse error
            }
    
            #ifdef ROM_THIRD_STAGE_DEBUG
            qDebug().noquote() << QString("%1[          RosBridgeClient::requestMappingMode response       ] : %2 %3")
                                   .arg(ROM_COLOR_GREEN).arg(status)
                                   .arg(ROM_COLOR_RESET);
            #endif

            mapping_mode_request_id.clear();
        }

        else if (!remapping_mode_request_id.isEmpty() && id == remapping_mode_request_id) 
        {
            #ifdef ROM_THIRD_STAGE_DEBUG
            qDebug() << "remapping response received." << obj;
            #endif

            const QJsonObject values = obj.value("values").toObject();

            //const int total_maps = values.value("total_maps").toInt(-1);
            const int status = values.value("status").toInt(-1);
            //const QString robot_namespace = values.value("robot_namespace").toString();
            //const QJsonArray map_names_arr = values.value("map_names").toArray();

            // QStringList map_names;
            // map_names.reserve(map_names_arr.size());
            // for (const QJsonValue &v : map_names_arr) map_names.push_back(v.toString());
            
            emit remapping_mode_response_received(status);
    
            #ifdef ROM_THIRD_STAGE_DEBUG
            qDebug().noquote() << QString("%1[          RosBridgeClient::requestremappingmode response       ] : %2 %3 ")
                                   .arg(ROM_COLOR_GREEN).arg(status)
                                   .arg(ROM_COLOR_RESET);
            #endif

            remapping_mode_request_id.clear();
        }

        else if (!which_maps_request_id.isEmpty() && id == which_maps_request_id) 
        {
            #ifdef ROM_THIRD_STAGE_DEBUG
            qDebug() << "whichMapsrequest response received." << obj;
            #endif

            const QJsonObject values = obj.value("values").toObject();

            const int total_maps = values.value("total_maps").toInt(-1);
            const int status = values.value("status").toInt(-1);

            const QString robot_namespace = values.value("robot_namespace").toString();
            const QJsonArray map_names_arr = values.value("map_names").toArray();

            QStringList map_names;
            map_names.reserve(map_names_arr.size());
            for (const QJsonValue &v : map_names_arr) map_names.push_back(v.toString());

            emit which_map_do_you_have_received(total_maps, map_names);
    
            qDebug() << "Which maps selected:";
            #ifdef ROM_THIRD_STAGE_DEBUG
            qDebug().noquote() << QString("%1[          RosBridgeClient::requestwhichmapsMode response       ]%2")
                                   .arg(ROM_COLOR_GREEN)
                                   .arg(QString("Total maps: %1, Map names: [%2], Robot namespace: %3, Status: %4")
                                        .arg(total_maps)
                                        .arg(map_names.join(", "))
                                        .arg(robot_namespace)
                                        .arg(status))
                                   .arg(ROM_COLOR_RESET);
            #endif

            which_maps_request_id.clear();
        }

        else if (!save_map_request_id.isEmpty() && id == save_map_request_id) 
        {
            #ifdef ROM_THIRD_STAGE_DEBUG
            qDebug() << "saveMap response received." << obj;
            #endif

            const QJsonObject values = obj.value("values").toObject();
            
            const int status = values.value("status").toInt(-2);

            if(status == -2)
            {
                qWarning() << "Failed to parse status in mapping mode response.";
                emit save_map_response_received(status);
            }
            else
            {
                emit save_map_response_received(status); // values to int parse error
            }
    
            #ifdef ROM_THIRD_STAGE_DEBUG
            qDebug().noquote() << QString("%1[          RosBridgeClient::requestSavemapMode response       ] : %2 %3 ")
                                   .arg(ROM_COLOR_GREEN).arg(status)
                                   .arg(ROM_COLOR_RESET);
            #endif

            save_map_request_id.clear();
        }

        else if (!select_map_request_id.isEmpty() && id == select_map_request_id) 
        {
           #ifdef ROM_THIRD_STAGE_DEBUG
            qDebug() << "selectMap response received." << obj;
            #endif

            const QJsonObject values = obj.value("values").toObject();

            const int status = values.value("status").toInt(-1);

            if(status == -2)
            {
                qWarning() << "Failed to parse status in mapping mode response.";
                emit select_map_response_received(status);
            }
            else
            {
                emit select_map_response_received(status); // values to int parse error
            }
            
            qDebug() << "Select map status:" << status;
            #ifdef ROM_THIRD_STAGE_DEBUG
            qDebug().noquote() << QString("%1[          RosBridgeClient::requestSelectMap response       ] : %2 %3 ")
                                   .arg(ROM_COLOR_GREEN).arg(status)
                                   .arg(ROM_COLOR_RESET);
            #endif

            select_map_request_id.clear();
        }
        
        else if (!add_waypoints_request_id.isEmpty() && id == add_waypoints_request_id) 
        {
            #ifdef ROM_THIRD_STAGE_DEBUG
            qDebug() << "get service points response received." << obj;
            #endif

            const QJsonObject values = obj.value("values").toObject();

            const int status = values.value("status").toInt(-2);

            emit add_waypoints_response_received(status);
    
            #ifdef ROM_THIRD_STAGE_DEBUG
            qDebug().noquote() << QString("%1[          RosBridgeClient:: response       ] : %2 %3 ")
                                   .arg(ROM_COLOR_GREEN).arg(status)
                                   .arg(ROM_COLOR_RESET);
            #endif

            add_waypoints_request_id.clear();
        }

        else if (!add_service_points_request_id.isEmpty() && id == add_service_points_request_id) 
        {

        }

        else if (!add_patrol_points_request_id.isEmpty() && id == add_patrol_points_request_id) 
        {

        }

        else if (!get_service_points_request_id.isEmpty() && id == get_service_points_request_id) 
        {
            #ifdef ROM_THIRD_STAGE_DEBUG
            qDebug() << "get service points response received." << obj;
            #endif

            const QJsonObject values = obj.value("values").toObject();

            const int status = values.value("status").toInt(-1);

            QVector<RomWaypoint> waypoints;
            const QJsonArray names = values.value("pose_names").toArray();
            const QJsonArray poses = values.value("poses").toArray();
            const QJsonArray scenePoses = values.value("scene_poses").toArray();

            int count = std::min({ names.size(), poses.size(), scenePoses.size() });
            waypoints.reserve(count);
            for (int i = 0; i < count; ++i) {
                RomWaypoint wp;
                wp.name = names.at(i).toString();

                QJsonObject poseStamped = poses.at(i).toObject();
                QJsonObject poseObj = poseStamped.value("pose").toObject();
                QJsonObject position = poseObj.value("position").toObject();
                QJsonObject orientation = poseObj.value("orientation").toObject();
                wp.world_x_m = position.value("x").toDouble();
                wp.world_y_m = position.value("y").toDouble();
                double oz = orientation.value("z").toDouble();
                double ow = orientation.value("w").toDouble();
                wp.world_theta_rad = 2.0 * std::atan2(oz, ow);

                QJsonObject sceneObj = scenePoses.at(i).toObject();
                QJsonObject scenePos = sceneObj.value("position").toObject();
                QJsonObject sceneOri = sceneObj.value("orientation").toObject();
                wp.image_x_px = scenePos.value("x").toDouble();
                wp.image_y_px = scenePos.value("y").toDouble();
                wp.image_theta_deg = sceneOri.value("w").toDouble();

                waypoints.append(wp);
            }

            emit service_points_received(waypoints);
    
            #ifdef ROM_THIRD_STAGE_DEBUG
            qDebug().noquote() << QString("%1[          RosBridgeClient:: response       ] : %2 %3 ")
                                   .arg(ROM_COLOR_GREEN).arg(status)
                                   .arg(ROM_COLOR_RESET);
            #endif

            get_service_points_request_id.clear();
        }
        
        else if (!get_patrol_points_request_id.isEmpty() && id == get_patrol_points_request_id) 
        {
            #ifdef ROM_THIRD_STAGE_DEBUG
            qDebug() << "get patrol points response received." << obj;
            #endif

            const QJsonObject values = obj.value("values").toObject();

            const int status = values.value("status").toInt(-1);

            QVector<RomWaypoint> patrol_points;
            const QJsonArray names = values.value("pose_names").toArray();
            const QJsonArray poses = values.value("poses").toArray();
            const QJsonArray scenePoses = values.value("scene_poses").toArray();

            int count = std::min({ names.size(), poses.size(), scenePoses.size() });
            patrol_points.reserve(count);
            for (int i = 0; i < count; ++i) 
            {
                RomWaypoint wp;
                wp.name = names.at(i).toString();

                QJsonObject poseStamped = poses.at(i).toObject();
                QJsonObject poseObj = poseStamped.value("pose").toObject();
                QJsonObject position = poseObj.value("position").toObject();
                QJsonObject orientation = poseObj.value("orientation").toObject();
                wp.world_x_m = position.value("x").toDouble();
                wp.world_y_m = position.value("y").toDouble();
                double oz = orientation.value("z").toDouble();
                double ow = orientation.value("w").toDouble();
                wp.world_theta_rad = 2.0 * std::atan2(oz, ow);

                QJsonObject sceneObj = scenePoses.at(i).toObject();
                QJsonObject scenePos = sceneObj.value("position").toObject();
                QJsonObject sceneOri = sceneObj.value("orientation").toObject();
                wp.image_x_px = scenePos.value("x").toDouble();
                wp.image_y_px = scenePos.value("y").toDouble();
                wp.image_theta_deg = sceneOri.value("w").toDouble();

                patrol_points.append(wp);
            }

            emit patrol_points_received(patrol_points);

            #ifdef ROM_THIRD_STAGE_DEBUG
            qDebug().noquote() << QString("%1[          RosBridgeClient:: response       ] : %2 %3 ")
                                   .arg(ROM_COLOR_GREEN).arg(status)
                                   .arg(ROM_COLOR_RESET);
            #endif

            get_patrol_points_request_id.clear();
        }
        
        else if (!get_waypoints_request_id.isEmpty() && id == get_waypoints_request_id) 
        {
            #ifdef ROM_THIRD_STAGE_DEBUG
            qDebug() << "get waypoints response received." << obj;
            #endif

            const QJsonObject values = obj.value("values").toObject();

            const int status = values.value("status").toInt(-1);
            // parse returned waypoint lists
            QVector<RomWaypoint> waypoints;
            const QJsonArray names = values.value("pose_names").toArray();
            const QJsonArray poses = values.value("poses").toArray();
            const QJsonArray scenePoses = values.value("scene_poses").toArray();

            int count = std::min({ names.size(), poses.size(), scenePoses.size() });
            waypoints.reserve(count);
            for (int i = 0; i < count; ++i) {
                RomWaypoint wp;
                wp.name = names.at(i).toString();

                // poses[i] is a PoseStamped-like object { header: {...}, pose: { position:{x,y,z}, orientation:{x,y,z,w} } }
                QJsonObject poseStamped = poses.at(i).toObject();
                QJsonObject poseObj = poseStamped.value("pose").toObject();
                QJsonObject position = poseObj.value("position").toObject();
                QJsonObject orientation = poseObj.value("orientation").toObject();
                wp.world_x_m = position.value("x").toDouble();
                wp.world_y_m = position.value("y").toDouble();
                // reconstruct yaw from quaternion stored as (z,w)
                double oz = orientation.value("z").toDouble();
                double ow = orientation.value("w").toDouble();
                wp.world_theta_rad = 2.0 * std::atan2(oz, ow);

                // scene_poses[i] is a Pose-like object { position:{x,y,z}, orientation:{x,y,z,w} }
                QJsonObject sceneObj = scenePoses.at(i).toObject();
                QJsonObject scenePos = sceneObj.value("position").toObject();
                QJsonObject sceneOri = sceneObj.value("orientation").toObject();
                wp.image_x_px = scenePos.value("x").toDouble();
                wp.image_y_px = scenePos.value("y").toDouble();
                wp.image_theta_deg = sceneOri.value("w").toDouble();

                waypoints.append(wp);
            }

            // emit parsed waypoints for UI
            emit waypoints_list_received(waypoints);
    
            #ifdef ROM_THIRD_STAGE_DEBUG
            qDebug().noquote() << QString("%1[          RosBridgeClient:: response       ] : %2 %3 ")
                                   .arg(ROM_COLOR_GREEN).arg(status)
                                   .arg(ROM_COLOR_RESET);
            #endif

            get_waypoints_request_id.clear();
        }
    }
}


void RosBridgeClient::subscribeCmdVel(const QString &topic) 
{

    if (!isConnected()) 
    {
        connectToServer();
    }

    cmd_vel_topic_name = m_robotNamespace + topic;
    QJsonObject msg;
    msg["op"] = "subscribe";
    msg["topic"] = cmd_vel_topic_name;
    // geometry_msgs/Twist message type; not strictly required but helpful
    msg["type"] = "geometry_msgs/msg/Twist"; // ROS 2 style type name
    sendJson(msg);
    m_cmdVelSubscribed = true;

    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[      RosBridgeClient::subscribeCmdVel      ] : %2 %3 ")
                              .arg(ROM_COLOR_GREEN).arg(cmd_vel_topic_name).arg(ROM_COLOR_RESET);
    #endif
}
void RosBridgeClient::unsubscribeCmdVel(const QString &topic) 
{
    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[     RosBridgeClient::unsubscribeCmdVel     ]%2")
                              .arg(ROM_COLOR_GREEN).arg(ROM_COLOR_RESET);
    #endif

    if (!isConnected() || !m_cmdVelSubscribed) return;
    
    QJsonObject msg;
    msg["op"] = "unsubscribe";
    msg["topic"] = cmd_vel_topic_name;
    sendJson(msg);
    m_cmdVelSubscribed = false;
}

void RosBridgeClient::subscribeMapBfp(const QString &topic)
{
    if (!isConnected())
    {
        connectToServer();
    }

    map_bfp_topic_name = m_robotNamespace + topic;
    QJsonObject msg;
    msg["op"] = "subscribe";
    msg["topic"] = map_bfp_topic_name;
    // geometry_msgs/TransformStamped message type; not strictly required but helpful
    msg["type"] = "geometry_msgs/msg/Pose2D"; // ROS 2 style type name
    sendJson(msg);
    m_mapBfpSubscribed = true;

    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[      RosBridgeClient::subscribeMapBfp      ] : %2 %3 ")
                              .arg(ROM_COLOR_GREEN).arg(map_bfp_topic_name).arg(ROM_COLOR_RESET);
    #endif
}
void RosBridgeClient::unsubscribeMapBfp(const QString &topic)
{
    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[     RosBridgeClient::unsubscribeMapBfp     ]%2")
                              .arg(ROM_COLOR_GREEN).arg(ROM_COLOR_RESET);
    #endif

    if (!isConnected() || !m_mapBfpSubscribed) return;

    QJsonObject msg;
    msg["op"] = "unsubscribe";
    msg["topic"] = map_bfp_topic_name;
    sendJson(msg);
    m_mapBfpSubscribed = false;
}

// ----------------- 20 Hz cmd_vel change-detect publisher API -----------------
void RosBridgeClient::setCmdVelPublishTopic(const QString &topic)
{
    cmd_vel_publish_topic_name = m_robotNamespace + topic; // + "/cmd_vel_qt_to_twist";
}
void RosBridgeClient::setDesiredCmdVel(const TwistData &twist)
{
    auto changed = [&](double a, double b, double eps){ return std::fabs(a - b) > eps; };
    if ( !changed(twist.linear_x, m_desiredTwist.linear_x, m_linEps) &&
         !changed(twist.angular_z, m_desiredTwist.angular_z, m_angEps) && 
         twist.linear_x == 0.0 && 
         twist.angular_z == 0.0)
    {
        m_desiredTwist = twist;
        m_cmdVelDirty = false;

        //enableCmdVelPublishing(false); // stop timer if zero timer ရပ်တာသေချာမသေချာ ပြန်စစ်ရန်
        
        #ifdef ROM_DEBUG
            qDebug().noquote() << QString("%1[          RosBridgeClient::setDesiredCmdVel          ] : dirty false %2")
                              .arg(ROM_COLOR_CYAN).arg(ROM_COLOR_RESET);
        #endif
    }
    else 
    {
        m_desiredTwist = twist;
        m_cmdVelDirty = true;

        //enableCmdVelPublishing(true); 

        #ifdef ROM_DEBUG
            qDebug().noquote() << QString("%1[          RosBridgeClient::setDesiredCmdVel          ] : dirty true %2")
                              .arg(ROM_COLOR_CYAN).arg(ROM_COLOR_RESET);
        #endif
    }
}
void RosBridgeClient::onCmdVelPubTimer()
{
    if (!isConnected()) return;
    if (!m_cmdVelDirty) return; // nothing changed since last publish

    // publish and clear dirty flag
    publishCmdVel(m_desiredTwist);
    m_lastPublishedTwist = m_desiredTwist;
    // m_cmdVelDirty = false;

        #ifdef ROM_DEBUG1
            qDebug().noquote() << QString("%1[          RosBridgeClient::onCmdVelPubTimer          ] : call publishCmdVel() %2")
            .arg(ROM_COLOR_CYAN).arg(ROM_COLOR_RESET);
        #endif
}
void RosBridgeClient::publishCmdVel(const TwistData &twist)
{
    if (!isConnected()) return;

    QJsonObject obj;
    obj["op"] = "publish";
    obj["topic"] = cmd_vel_publish_topic_name;

    QJsonObject lin;
    lin["x"] = twist.linear_x;
    lin["y"] = twist.linear_y;
    lin["z"] = twist.linear_z;
    QJsonObject ang;
    ang["x"] = twist.angular_x;
    ang["y"] = twist.angular_y;
    ang["z"] = twist.angular_z;
    QJsonObject msg;
    msg["linear"] = lin;
    msg["angular"] = ang;
    obj["msg"] = msg;
    sendJson(obj);

    #ifdef ROM_DEBUG1
        int hz = 0;
        QTime now = QTime::currentTime();
        if (previous_cmdvel_publish_time.isValid()) {
            int elapsed = previous_cmdvel_publish_time.msecsTo(now);
            previous_cmdvel_publish_time = now;
            if (elapsed > 0) hz = 1000 / elapsed;
        } else {
            previous_cmdvel_publish_time = now;
        }
        qDebug().noquote() << QString("%1[           RosBridgeClient::publishCmdVel            ] : [ %2 Hz ] topic=%3 lin=(%4) ang=(%5) %6")
                              .arg(ROM_COLOR_YELLOW)
                              .arg(hz)
                              .arg(cmd_vel_publish_topic_name)
                              .arg(twist.linear_x, 0, 'f', 3)
                              .arg(twist.angular_z, 0, 'f', 3)
                              .arg(ROM_COLOR_RESET);
    #endif
}
void RosBridgeClient::enableCmdVelPublishing(bool enable)
{
    if (enable) 
    {
        if (!m_cmdVelPubTimer.isActive()) m_cmdVelPubTimer.start();
    } 
    else 
    {
        if (m_cmdVelPubTimer.isActive()) m_cmdVelPubTimer.stop();
    }
}


void RosBridgeClient::subscribeMap(const QString &topic) 
{
    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[       RosBridgeClient::subscribeMap        ]%2")
                              .arg(ROM_COLOR_GREEN).arg(ROM_COLOR_RESET);
    #endif
    
    if (!isConnected()) 
    {
        connectToServer();
    }
    
    map_topic_name = m_robotNamespace + topic;
    QJsonObject msg;
    msg["op"] = "subscribe";
    msg["topic"] = map_topic_name;
    // nav_msgs/OccupancyGrid (ROS2 msg type style)
    msg["type"] = "nav_msgs/msg/OccupancyGrid";
    sendJson(msg);
    m_mapSubscribed = true;

    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[       RosBridgeClient::subscribeMap        ] : %2 %3 ")
                              .arg(ROM_COLOR_GREEN).arg(map_topic_name).arg(ROM_COLOR_RESET);
    #endif
    
    //requestHSOnce("/which_name"); // which_map, which_nav, which_vels with trigger
    //requestMapOnce("/map_server/map"); // 
}
void RosBridgeClient::unsubscribeMap(const QString &topic) 
{
    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[      RosBridgeClient::unsubscribeMap      ]%2")
                              .arg(ROM_COLOR_GREEN).arg(ROM_COLOR_RESET);
    #endif
    if (!isConnected() || !m_mapSubscribed) return;
    QJsonObject msg;
    msg["op"] = "unsubscribe";
    msg["topic"] = map_topic_name;
    sendJson(msg);
    m_mapSubscribed = false;
}


void RosBridgeClient::requestMapOnce(const QString &service) 
{
    // Prepare a rosbridge call_service request for nav_msgs/srv/GetMap
    if (!isConnected()) 
    {
        connectToServer();
    }

    m_mapServiceName = service;
    m_mapRequestId = QStringLiteral("get_map_%1").arg(QDateTime::currentMSecsSinceEpoch());

    QJsonObject req;
    req["op"] = "call_service";
    req["service"] = m_mapServiceName;
    // For ROS 2 GetMap, no args required
    req["args"] = QJsonObject{};
    req["id"] = m_mapRequestId;
    sendJson(req);

    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[      RosBridgeClient::requestMapOnce       ] : service=%2 id=%3 %4")
                              .arg(ROM_COLOR_GREEN)
                              .arg(m_mapServiceName)
                              .arg(m_mapRequestId)
                              .arg(ROM_COLOR_RESET);
    #endif
}
void RosBridgeClient::requestHSOnce(const QString &service) 
{
    // Prepare a rosbridge call_service request for nav_msgs/srv/GetMap
    if (!isConnected()) 
    {
        connectToServer();
    }
    
    m_hsRequestId = QStringLiteral("get_HS_%1").arg(QDateTime::currentMSecsSinceEpoch());

    QJsonObject req;
    req["op"] = "call_service";
    req["service"] = service;
    // For ROS 2 GetMap, no args required
    QJsonObject args;
    args["request_string"] = "handshake";
    args["map_name_to_save"] = "";
    args["map_name_to_select"] = "";
    args["login_access_token"] = "*#5447972162718281828459#";
    req["args"] = args;
    req["id"] = m_hsRequestId;
    sendJson(req);

    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[      RosBridgeClient::requestMapOnce       ] : service=%2 id=%3 %4")
                              .arg(ROM_COLOR_GREEN)
                              .arg(service)
                              .arg(m_hsRequestId)
                              .arg(ROM_COLOR_RESET);
    #endif
}


void RosBridgeClient::subscribeTF(const QString &topic) 
{
    if (!isConnected()) {
        connectToServer();
    }
    tf_topic_name = m_robotNamespace + topic;
    QJsonObject msg;
    msg["op"] = "subscribe";
    msg["topic"] = tf_topic_name;
    // tf2_msgs/TFMessage (ROS2 style)
    msg["type"] = "tf2_msgs/msg/TFMessage";
    sendJson(msg);
    m_tfSubscribed = true;
}
void RosBridgeClient::unsubscribeTF(const QString &topic) 
{
    if (!isConnected() || !m_tfSubscribed) return;
    QJsonObject msg;
    msg["op"] = "unsubscribe";
    msg["topic"] = tf_topic_name;
    sendJson(msg);
    m_tfSubscribed = false;
}


void RosBridgeClient::subscribeFrontLaserScan(const QString &topic) 
{
    if (!isConnected()) {
        connectToServer();
    }
    front_laser_scan_topic_name = m_robotNamespace + topic;
    QJsonObject msg;
    msg["op"] = "subscribe";
    msg["topic"] = front_laser_scan_topic_name;
    msg["type"] = "sensor_msgs/msg/LaserScan";
    sendJson(msg);
    m_frontLaserSubscribed = true;
}
void RosBridgeClient::unsubscribeFrontLaserScan(const QString &topic) 
{
    if (!isConnected() || !m_frontLaserSubscribed) return;
    QJsonObject msg;
    msg["op"] = "unsubscribe";
    msg["topic"] = front_laser_scan_topic_name;
    sendJson(msg);
    m_frontLaserSubscribed = false;
}

// void RosBridgeClient::subscribeRearLaserScan(const QString &topic) {
//     if (!isConnected()) {
//         connectToServer();
//     }
//     rear_laser_scan_topic_name = m_robotNamespace + topic;
//     QJsonObject msg;
//     msg["op"] = "subscribe";
//     msg["topic"] = rear_laser_scan_topic_name;
//     msg["type"] = "sensor_msgs/msg/LaserScan";
//     sendJson(msg);
//     m_rearLaserSubscribed = true;
// }
// void RosBridgeClient::unsubscribeRearLaserScan(const QString &topic) {
//     if (!isConnected() || !m_rearLaserSubscribed) return;
//     QJsonObject msg;
//     msg["op"] = "unsubscribe";
//     msg["topic"] = rear_laser_scan_topic_name;
//     sendJson(msg);
//     m_rearLaserSubscribed = false;
// }


void RosBridgeClient::subscribeOdom(const QString &topic) 
{
    if (!isConnected()) {
        connectToServer();
    }
    odom_topic_name = m_robotNamespace + topic;
    QJsonObject msg;
    msg["op"] = "subscribe";
    msg["topic"] = odom_topic_name;
    msg["type"] = "nav_msgs/msg/Odometry";
    sendJson(msg);
    m_odomSubscribed = true;
}
void RosBridgeClient::unsubscribeOdom() 
{
    if (!isConnected() || !m_odomSubscribed) return;
    QJsonObject msg;
    msg["op"] = "unsubscribe";
    msg["topic"] = odom_topic_name;
    sendJson(msg);
    m_odomSubscribed = false;
}

void RosBridgeClient::subscribeControllerOdom(const QString &topic) 
{
    if (!isConnected()) {
        connectToServer();
    }
    controller_odom_topic_name = m_robotNamespace + topic;
    QJsonObject msg;
    msg["op"] = "subscribe";
    msg["topic"] = controller_odom_topic_name;
    msg["type"] = "nav_msgs/msg/Odometry";
    sendJson(msg);
    m_controllerOdomSubscribed = true;
}
void RosBridgeClient::unsubscribeControllerOdom() 
{
    if (!isConnected() || !m_controllerOdomSubscribed) return;
    QJsonObject msg;
    msg["op"] = "unsubscribe";
    msg["topic"] = controller_odom_topic_name;
    sendJson(msg);
    m_controllerOdomSubscribed = false;
}

// void RosBridgeClient::subscribeFusedOdom(const QString &topic) {
//     // Alias to subscribeOdom
//     subscribeOdom(topic);
// }

// void RosBridgeClient::unsubscribeFusedOdom(const QString &topic) {
//     Q_UNUSED(topic);
//     unsubscribeOdom();
// }

void RosBridgeClient::requestNavigationMode(const QString &service)
{
    if (!isConnected()) 
    {
        connectToServer();
    }
    
    navi_mode_request_id = QStringLiteral("get_navi_%1").arg(QDateTime::currentMSecsSinceEpoch());
    which_map_srv_ns = "";
    which_map_srv_ns = m_robotNamespace + service;
    
    QJsonObject req;
    req["op"] = "call_service";
    req["service"] = which_map_srv_ns;
    // For ROS 2 GetMap, no args required
    QJsonObject args;
    args["request_string"] = "navi";
    args["map_name_to_save"] = "";
    args["map_name_to_select"] = "";
    args["login_access_token"] = "";
    req["args"] = args;
    req["id"] = navi_mode_request_id;
    sendJson(req);

    #ifdef ROM_THIRD_STAGE_DEBUG
        qDebug().noquote() << QString("%1[   RosBridgeClient::requestNavigationMode   ] : service=%2 id=%3 %4")
                              .arg(ROM_COLOR_GREEN)
                              .arg(which_map_srv_ns)
                              .arg(navi_mode_request_id)
                              .arg(ROM_COLOR_RESET);
    #endif
}

void RosBridgeClient::requestMappingMode(const QString &service)
{
    if (!isConnected()) 
    {
        connectToServer();
    }

    mapping_mode_request_id = QStringLiteral("get_map_%1").arg(QDateTime::currentMSecsSinceEpoch());
    which_map_srv_ns = "";
    which_map_srv_ns = m_robotNamespace + service;

    QJsonObject req;
    req["op"] = "call_service";
    req["service"] = which_map_srv_ns;
    // For ROS 2 GetMap, no args required
    QJsonObject args;
    args["request_string"] = "mapping";
    args["map_name_to_save"] = "";
    args["map_name_to_select"] = "";
    args["login_access_token"] = "";
    req["args"] = args;
    req["id"] = mapping_mode_request_id;
    sendJson(req);

    #ifdef ROM_THIRD_STAGE_DEBUG
        qDebug().noquote() << QString("%1[    RosBridgeClient::requestMappingMode     ] : service=%2 id=%3 %4")
                              .arg(ROM_COLOR_GREEN)
                              .arg(which_map_srv_ns)
                              .arg(mapping_mode_request_id)
                              .arg(ROM_COLOR_RESET);
    #endif
}

void RosBridgeClient::requestReMappingMode(const QString &service)
{
    if (!isConnected()) 
    {
        connectToServer();
    }

    remapping_mode_request_id = QStringLiteral("get_re_%1").arg(QDateTime::currentMSecsSinceEpoch());
    which_map_srv_ns = "";
    which_map_srv_ns = m_robotNamespace + service;

    QJsonObject req;
    req["op"] = "call_service";
    req["service"] = which_map_srv_ns;
    // For ROS 2 GetMap, no args required
    QJsonObject args;
    args["request_string"] = "remapping";
    args["map_name_to_save"] = "";
    args["map_name_to_select"] = "";
    args["login_access_token"] = "";
    req["args"] = args;
    req["id"] = remapping_mode_request_id;
    sendJson(req);

    #ifdef ROM_THIRD_STAGE_DEBUG
        qDebug().noquote() << QString("%1[   RosBridgeClient::requestReMappingMode    ] : service=%2 id=%3 %4")
                              .arg(ROM_COLOR_GREEN)
                              .arg(which_map_srv_ns)
                              .arg(remapping_mode_request_id)
                              .arg(ROM_COLOR_RESET);
    #endif
}

void RosBridgeClient::requestWhichMapsDoYouHave(const QString &service)
{
    if (!isConnected()) 
    {
        connectToServer();
    }

    which_maps_request_id = QStringLiteral("get_doyo_%1").arg(QDateTime::currentMSecsSinceEpoch());
    which_map_srv_ns = "";

    which_map_srv_ns = m_robotNamespace + service;
    
    QJsonObject req;
    req["op"] = "call_service";
    req["service"] = which_map_srv_ns;
    // For ROS 2 GetMap, no args required
    QJsonObject args;
    args["request_string"] = "which_maps_do_you_have";
    args["map_name_to_save"] = "";
    args["map_name_to_select"] = "";
    args["login_access_token"] = "";
    req["args"] = args;
    req["id"] = which_maps_request_id;
    sendJson(req);
    qDebug()<<"requested which maps do you have";
    #ifdef ROM_THIRD_STAGE_DEBUG
        qDebug().noquote() << QString("%1[ RosBridgeClient::requestWhichMapsDoYouHave ] : service=%2 id=%3 %4")
                              .arg(ROM_COLOR_GREEN)
                              .arg(which_map_srv_ns)
                              .arg(which_maps_request_id)
                              .arg(ROM_COLOR_RESET);
    #endif
}

void RosBridgeClient::requestSaveMap(const QString &service, const QString &save_name)
{
    qDebug()<<"requested SaveMap";
    if (!isConnected()) 
    {
        connectToServer();
    }

    save_map_request_id = QStringLiteral("get_savmap_%1").arg(QDateTime::currentMSecsSinceEpoch());
    which_map_srv_ns = "";
    which_map_srv_ns = m_robotNamespace + service;

    QJsonObject req;
    req["op"] = "call_service";
    req["service"] = which_map_srv_ns;
    // For ROS 2 GetMap, no args required
    QJsonObject args;
    args["request_string"] = "save_map";
    args["map_name_to_save"] = save_name;
    args["map_name_to_select"] = "";
    args["login_access_token"] = "";
    req["args"] = args;
    req["id"] = save_map_request_id;
    sendJson(req);

    #ifdef ROM_THIRD_STAGE_DEBUG
        qDebug().noquote() << QString("%1[      RosBridgeClient::requestSaveMap       ] : service=%2 id=%3 %4")
                              .arg(ROM_COLOR_GREEN)
                              .arg(which_map_srv_ns)
                              .arg(save_map_request_id)
                              .arg(ROM_COLOR_RESET);
    #endif
}

void RosBridgeClient::requestSelectMap(const QString &service, const QString &open_name)
{
    if (!isConnected()) 
    {
        connectToServer();
    }

    select_map_request_id = QStringLiteral("get_selmap_%1").arg(QDateTime::currentMSecsSinceEpoch());
    which_map_srv_ns = "";
    which_map_srv_ns = m_robotNamespace + service;

    QJsonObject req;
    req["op"] = "call_service";
    req["service"] = which_map_srv_ns;
    // For ROS 2 GetMap, no args required
    QJsonObject args;
    args["request_string"] = "select_map";
    args["map_name_to_save"] = "";
    args["map_name_to_select"] = open_name;
    args["login_access_token"] = "";
    req["args"] = args;
    req["id"] = select_map_request_id;
    sendJson(req);

    #ifdef ROM_THIRD_STAGE_DEBUG
        qDebug().noquote() << QString("%1[     RosBridgeClient::requestSelectMap      ] : service=%2 id=%3 %4")
                              .arg(ROM_COLOR_GREEN)
                              .arg(which_map_srv_ns)
                              .arg(select_map_request_id)
                              .arg(ROM_COLOR_RESET);
    #endif
}


void RosBridgeClient::addWaypointsList(QVector<RomWaypoint> waypoints_vector)
{
    if (!isConnected()) 
    {
        connectToServer();
    }

    add_waypoints_request_id = QStringLiteral("get_wp_%1").arg(QDateTime::currentMSecsSinceEpoch());
    construct_yml_srv_ns = "";
    construct_yml_srv_ns = m_robotNamespace + "/construct_yaml_and_bt";

    QJsonArray poseNames;
    QJsonArray poses;        // geometry_msgs/PoseStamped[]
    QJsonArray scenePoses;   // geometry_msgs/Pose[]

    for (const auto &wp : waypoints_vector) 
    {
        // 1) pose_names
        poseNames.append(wp.name);

        // 2) PoseStamped (header + pose)
        QJsonObject header;
        {
            QJsonObject stamp;
            stamp["sec"] = 0;
            stamp["nanosec"] = 0;
            header["stamp"] = stamp;
            header["frame_id"] = "map";
        }

        // Convert yaw (rad) -> quaternion (z,w)
        const double half = wp.world_theta_rad * 0.5;
        const double cz = std::cos(half);
        const double sz = std::sin(half);

        QJsonObject position;
        position["x"] = wp.world_x_m;
        position["y"] = wp.world_y_m;
        position["z"] = 0.0;

        QJsonObject orientation;
        orientation["x"] = 0.0;
        orientation["y"] = 0.0;
        orientation["z"] = sz;
        orientation["w"] = cz;

        QJsonObject pose;
        pose["position"] = position;
        pose["orientation"] = orientation;

        QJsonObject poseStamped;
        poseStamped["header"] = header;
        poseStamped["pose"] = pose;

        poses.append(poseStamped);

        // 3) scene_poses (just Pose, no header)
        QJsonObject scenePose;

        QJsonObject scenePosition;
        scenePosition["x"] = wp.image_x_px;
        scenePosition["y"] = wp.image_y_px;
        scenePosition["z"] = 0.0;

        QJsonObject sceneOrientation;
        sceneOrientation["x"] = 0.0;
        sceneOrientation["y"] = 0.0;
        sceneOrientation["z"] = 0.0;
        sceneOrientation["w"] = wp.image_theta_deg;


        scenePose["position"] = scenePosition;
        scenePose["orientation"] = sceneOrientation;
        scenePoses.append(scenePose);
    }

    // Build args in your existing format
    QJsonObject args;
    args["pose_names"]  = poseNames;
    args["poses"]       = poses;
    args["scene_poses"] = scenePoses;

    //waypoints_mode, service_mode, patrol_mode, goal_mode ( three ros2 run commands), eraser_mode, get_wp_list, get_srv_list, get_patrol_list
    args["mode"]        = "waypoints_mode";  // or service_mode / patrol_mode / goal_mode / eraser_mode
    args["loop"]        = false;             // set true if you want looping behavior
    args["command"]     = "";                //  for your ros2 runtime command selector

    QJsonObject req;
    req["op"]      = "call_service";
    req["service"] = construct_yml_srv_ns;
    // Optional but useful for ROS 2 rosbridge type disambiguation:
    req["type"] = "rom_interfaces/srv/ConstructYaml";
    req["args"]    = args;
    req["id"]      = add_waypoints_request_id;

    sendJson(req);

    #ifdef ROM_THIRD_STAGE_DEBUG
        qDebug().noquote() << QString("%1[     RosBridgeClient::addWaypointsList      ] : service=%2 id=%3 %4")
                              .arg(ROM_COLOR_GREEN)
                              .arg(construct_yml_srv_ns)
                              .arg(add_waypoints_request_id)
                              .arg(ROM_COLOR_RESET);
    #endif

}

// addServicePointsList, addPatrolPointsList can be similar to addWaypointsList

void RosBridgeClient::getWaypointsList()
{
    if (!isConnected()) 
    {
        connectToServer();
    }

    get_waypoints_request_id = QStringLiteral("get_wp_%1").arg(QDateTime::currentMSecsSinceEpoch());
    construct_yml_srv_ns = "";
    construct_yml_srv_ns = m_robotNamespace + "/construct_yaml_and_bt";

    QJsonArray poseNames;
    QJsonArray poses;        // geometry_msgs/PoseStamped[]
    QJsonArray scenePoses;   // geometry_msgs/Pose[]

    // Build args in your existing format
    QJsonObject args;
    args["pose_names"]  = poseNames;
    args["poses"]       = poses;
    args["scene_poses"] = scenePoses;

    //waypoints_mode, service_mode, patrol_mode, goal_mode ( three ros2 run commands), eraser_mode, get_wp_list, get_srv_list, get_patrol_list
    args["mode"]        = "get_wp_list";  // or service_mode / patrol_mode / goal_mode / eraser_mode
    args["loop"]        = false;             // set true if you want looping behavior
    args["command"]     = "";                //  for your ros2 runtime command selector

    QJsonObject req;
    req["op"]      = "call_service";
    req["service"] = construct_yml_srv_ns;
    // Optional but useful for ROS 2 rosbridge type disambiguation:
    req["type"] = "rom_interfaces/srv/ConstructYaml";
    req["args"]    = args;
    req["id"]      = get_waypoints_request_id;

    sendJson(req);

    #ifdef ROM_THIRD_STAGE_DEBUG
        qDebug().noquote() << QString("%1[     RosBridgeClient::getWaypointsList      ] : service=%2 id=%3 %4")
                              .arg(ROM_COLOR_GREEN)
                              .arg(construct_yml_srv_ns)
                              .arg(get_waypoints_request_id)
                              .arg(ROM_COLOR_RESET);
    #endif

}


void RosBridgeClient::goAllWaypoints()
{
    if (!isConnected()) 
    {
        connectToServer();
    }

    go_all_waypoints_request_id = QStringLiteral("go_all_wp_%1").arg(QDateTime::currentMSecsSinceEpoch());
    go_all_waypoints_srv_ns = "";
    go_all_waypoints_srv_ns = m_robotNamespace + "/construct_yaml_and_bt";

    QJsonArray poseNames;
    QJsonArray poses;        // geometry_msgs/PoseStamped[]
    QJsonArray scenePoses;

    QJsonObject args;
    args["pose_names"]  = poseNames;
    args["poses"]       = poses;
    args["scene_poses"] = scenePoses;

    //waypoints_mode, service_mode, patrol_mode, goal_mode ( three ros2 run commands), eraser_mode, get_wp_list, get_srv_list, get_patrol_list
    args["mode"]        = "goal_mode";  // or service_mode / patrol_mode / goal_mode / eraser_mode
    args["loop"]        = false;             // set true if you want looping behavior
    args["command"]     = "ros2 run rom_robot_autonomy autonomy_waypoints_mode_w_cancel";                //  for your ros2 runtime command selector

    QJsonObject req;
    req["op"]      = "call_service";
    req["service"] = go_all_waypoints_srv_ns;
    // Optional but useful for ROS 2 rosbridge type disambiguation:
    req["type"] = "rom_interfaces/srv/ConstructYaml";
    req["args"]    = args;
    req["id"]      = go_all_waypoints_request_id;


    sendJson(req);

    #ifdef ROM_THIRD_STAGE_DEBUG
        qDebug().noquote() << QString("%1[     RosBridgeClient::goAllWaypoints       ] : service=%2 id=%3 %4")
                              .arg(ROM_COLOR_GREEN)
                              .arg(go_all_waypoints_srv_ns)
                              .arg(go_all_waypoints_request_id)
                              .arg(ROM_COLOR_RESET);
    #endif
}