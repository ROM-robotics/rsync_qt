#pragma once
#include <QObject>
#include <QWebSocket>
#include <QTimer>
#include <QTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "rom_map.hpp"
#include "rom_tf.hpp"
#include "rom_sensors.hpp"
#include "rom_odom.hpp"
//#include "rom_robot.hpp"


class RosBridgeClient : public QObject {
    Q_OBJECT
public:
    explicit RosBridgeClient(const QString &robot_ns="", 
        const QString &host = "127.0.0.1", 
        quint16 port = 9090, 
        QObject *parent = nullptr);

    void connectToServer();
    void disconnectFromServer();
    bool isConnected() const { return m_socket.state() == QAbstractSocket::ConnectedState; }

    // --------------------------------- TOPIC SUBSCRIPTIONS
    void subscribeMap(const QString &topic = "/map");
    void unsubscribeMap(const QString &topic = "/map");

    void subscribeCmdVel(const QString &topic = "/diff_controller/cmd_vel_unstamped");
    void unsubscribeCmdVel(const QString &topic = "/diff_controller/cmd_vel_unstamped");

    void subscribeMapBfp(const QString &topic = "/map_bfp_publisher");
    void unsubscribeMapBfp(const QString &topic = "/map_bfp_publisher");

    // 20 Hz publisher (publishes only when data changes)
    QString cmd_vel_publish_topic_name;
    QTime previous_cmdvel_publish_time;
    void publishCmdVel(const TwistData &twist);
    void setDesiredCmdVel(const TwistData &twist);
    void setCmdVelPublishTopic(const QString &topic);
    void enableCmdVelPublishing(bool enable = true);

    void subscribeFusedOdom(const QString &topic = "/odom");
    void unsubscribeFusedOdom(const QString &topic = "/odom");
    
    void subscribeOdom(const QString &topic = "/odom");
    void unsubscribeOdom();

    void subscribeControllerOdom(const QString &topic = "/diff_controller/odom");
    void unsubscribeControllerOdom();
    
    void subscribeFrontLaserScan(const QString &topic = "/scan");
    void unsubscribeFrontLaserScan(const QString &topic = "/scan");

    //void subscribeRearLaserScan(const QString &topic = "/scan");
    //void unsubscribeRearLaserScan(const QString &topic = "/scan");

    void subscribeTF(const QString &topic = "/tf");
    void unsubscribeTF(const QString &topic = "/tf");

    void subscribeWaypointsList(const QString &topic = "/waypoints_list");
    void unsubscribeWaypointsList(const QString &topic = "/waypoints_list");

    // --------------------------------- SERVICE CALLS
    // Fallback: request OccupancyGrid via service once (ROS 2 map_server exposes /map GetMap)
    void requestMapOnce(const QString &service = "/map");
    void requestHSOnce(const QString &service = "/which_name");

    void requestNavigationMode(const QString &service = "/which_maps");
    void requestMappingMode(const QString &service = "/which_maps");
    void requestReMappingMode(const QString &service = "/which_maps");

    void requestWhichMapsDoYouHave(const QString &service = "/which_maps");
    void requestSaveMap(const QString &service = "/which_maps", const QString &save_name = "");
    void requestSelectMap(const QString &service = "/which_maps", const QString &open_name = "");

    void addWaypointsList(QVector<RomWaypoint> waypoints_vector);
    //void addWaypointsList(QVector<RomWaypoint> waypoints_vector);
    //void addWaypointsList(QVector<RomWaypoint> waypoints_vector);

    void getWaypointsList();
    //void getServicePointsList();
    //void getPatrolPointsList();

    void goAllWaypoints();

    
    
    // which_vel

    // to do
    // qt_send_wp_client, qt_wp_receiver, /robot_emotion_command, robot_eyes_node, tf_listener_node
    // robot_pose_publisher, qt_map_receiver, qt_laser_receiver, qt_goal_action_client_node
    // qt_construce_yaml_client, qt_cmd_service_client

    // --------------------------------- TF

signals:
    void connected();
    void disconnected();
    void errorOccurred(const QString &msg);

    void twistReceived(const TwistData &data, qint64 timestampUsec);
    void mapBfpReceived(const RomPose2D &data, qint64 timestampUsec);
    void mapReceived(const RomMap &map);

    void tfReceived(const RomTF &data, qint64 timestampUsec);
    void laserFrontReceived(const RomLaser &data, qint64 timestampUsec);
    //void laserRearReceived(const RomLaser &data, qint64 timestampUsec);

    void odomReceived(const RomOdom &data, qint64 timestampUsec);
    void controllerOdomReceived(const RomOdom &data, qint64 timestampUsec);

    // which_maps sdk
    void which_map_do_you_have_received(const int total, const QStringList &data);
    void save_map_response_received(const int &response);
    void select_map_response_received(const int &response);

    void navigation_mode_response_received(const int &response);
    void mapping_mode_response_received(const int &response);
    void remapping_mode_response_received(const int &response);

    // constructl_yaml_and_bt sdk
    void waypoints_list_received(const QVector<RomWaypoint> &data);
    void service_points_received(const QVector<RomWaypoint> &data);
    void patrol_points_received(const QVector<RomWaypoint> &data);

    void add_waypoints_response_received(const int &response);
    //void add_service_points_response_received(const int &response);
    //void add_patrol_points_response_received(const int &response);

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError err);
    
    void onTextMessageReceived(const QString &msg);

    // 20 Hz tick handler for cmd_vel publishing on change
    void onCmdVelPubTimer();
protected:
        RomTF global_tfData;
        bool emit_tf = false;

private:
    void sendJson(const QJsonObject &obj);
    void ensureReconnect();

    QWebSocket m_socket;
    QString m_robotNamespace;
    QString m_host;
    quint16 m_port{9090};
    QTimer m_reconnectTimer;
    QTimer m_cmdVelPubTimer; // 20 Hz publisher timer

    bool m_cmdVelSubscribed{false};
    QString cmd_vel_topic_name;

    bool m_mapSubscribed{false};
    QString map_topic_name;
    QString m_mapServiceName;
    QString m_mapRequestId;

    bool m_tfSubscribed{false};
    QString tf_topic_name;

    bool m_frontLaserSubscribed{false};
    QString front_laser_scan_topic_name;

    //bool m_rearLaserSubscribed{false};
    QString rear_laser_scan_topic_name = "/rear/scan";
    
    bool m_odomSubscribed{false};
    QString odom_topic_name;

    bool m_controllerOdomSubscribed{false};
    QString controller_odom_topic_name;

    QString m_hsRequestId;
    QString navi_mode_request_id, mapping_mode_request_id, remapping_mode_request_id;
    QString which_maps_request_id, save_map_request_id, select_map_request_id;

    bool m_mapBfpSubscribed{false};
    QString map_bfp_topic_name;

    QString which_map_srv_ns;

    QString get_waypoints_request_id, get_service_points_request_id, get_patrol_points_request_id;
    QString add_waypoints_request_id, add_service_points_request_id, add_patrol_points_request_id;
    QString construct_yml_srv_ns , go_all_waypoints_srv_ns , go_all_service_points_srv_ns , go_all_patrol_points_srv_ns ;

    QString go_all_waypoints_request_id, go_all_service_points_request_id, go_all_patrol_points_request_id;
    

    QString imu_topic_name;
    QString battery_topic_name;
    QString camera_topic_name;
    QString camera_info_topic_name;
    QString camera_depth_topic_name;
    QString camera_rgb_topic_name;
    QString camera_ir_topic_name;
    QString camera_pointcloud_topic_name;
    QString camera_pointcloud2_topic_name;

    // State for cmd_vel change-detect publishing
    TwistData m_desiredTwist{};
    TwistData m_lastPublishedTwist{};
    bool m_cmdVelDirty{false};
    double m_linEps{1e-4};
    double m_angEps{1e-4};

    
};
