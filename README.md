# rsync_qt
tuning_app မှာ ဒါမျိုးရေးထားတာမို့လို့ 
```cpp
// If the current Linux user is "mr_robot", prefer the fixed path under /home/mr_robot/data/app
    const QString user = QString::fromLocal8Bit(qgetenv("USER"));
    if (user == "mr_robot" || user == "jackal" || user == "zyme") 
    {
        QString preferred = "";
        if( user == "mr_robot" )    {  preferred = "/home/mr_robot/data/app/rsync_qt/mr_robot/apprsync_qt";   }
        else if( user == "jackal" ) {  preferred = "/home/jackal/data/app/rsync_qt/jackal/apprsync_qt";   }
        else if( user == "zyme" )   {  preferred = "/home/zyme/data/app/rsync_qt/zyme/apprsync_qt";   }
...
```
appImage ကို အမည်ပြောင်းပြီး /home/mr_robot/data/app/rsync_qt/mr_robot/ path ထဲထည့်ထားသင့်ပါတယ်။