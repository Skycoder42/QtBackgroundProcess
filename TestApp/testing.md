# How to test:
Do the following commands and compare with the output to validate:

## Simple Test

### Commands
1. `start Test1`
2. `Test2`
3. `start Test 3`
4. `stop Test 4`

### Outputs
#### Terminal 1
```
```

#### Terminal 2
```
```

#### Terminal 3
```
[Warning]  QBackgroundProcess: Master is already running. Start arguments will be passed to it as is
```

#### Terminal 4
```
```

#### Log File
```
[2016-12-10T15:05:43 Debug]    App Master Started with arguments: ("__qbckgrndprcss$start#master~", "Test1") and options: ()
[2016-12-10T15:05:43 Debug]    skipping starter args: ("start", "Test1") and options: ()
[2016-12-10T15:05:49 Debug]    received new command: ("Test2") and options: ()
[2016-12-10T15:05:56 Debug]    received new command: ("start", "Test", "3") and options: ()
[2016-12-10T15:06:01 Debug]    received new command: ("stop", "Test", "4") and options: ()
[2016-12-10T15:06:01 Debug]    stop requested with ("stop", "Test", "4") and options: ()
```

## Commands Test

### Commands
1. `Test1`
2. `-a -f 1 -i Test2`
3. `start -i Test 3`
4. `start Test 4`
5. `-a -i Test5`
6. `-i Test6`
7. `-f 0 Test 7`
8. `Test 8`
9. `stop -f 1 Test9`

### Outputs
#### Terminal 1
```
[Critical] QBackgroundProcess: Master process is not running! Please launch it by using: ".../TestApp.exe start"
```

#### Terminal 2
##### After Command
```
[2016-12-10T15:07:26 Debug]    App Master Started with arguments: ("__qbckgrndprcss$start#master~", "Test2") and options: ("a", "f", "i")
[2016-12-10T15:07:26 Debug]    skipping starter args: ("Test2") and options: ("a", "f", "i")
```

##### Completely
```
[2016-12-10T15:07:26 Debug]    App Master Started with arguments: ("__qbckgrndprcss$start#master~", "Test2") and options: ("a", "f", "i")
[2016-12-10T15:07:26 Debug]    skipping starter args: ("Test2") and options: ("a", "f", "i")
[2016-12-10T15:07:56 Debug]    received new command: () and options: ()
[2016-12-10T15:08:04 Debug]    received new command: ("start", "Test", "4") and options: ()
[2016-12-10T15:08:10 Debug]    received new command: ("Test5") and options: ("a", "i")
[2016-12-10T15:08:17 Debug]    received new command: ("Test6") and options: ("i")
[2016-12-10T15:08:38 Debug]    received new command: ("stop", "Test") and options: ("f")
```

#### Terminal 3
##### After Command
```
[Warning]  QBackgroundProcess: Start commands ignored because master is already running! The terminal will connect with an empty argument list!
[2016-12-10T15:07:56 Debug]    received new command: () and options: ()
```

##### Completely
```
[Warning]  QBackgroundProcess: Start commands ignored because master is already running! The terminal will connect with an empty argument list!
[2016-12-10T15:07:56 Debug]    received new command: () and options: ()
[2016-12-10T15:08:04 Debug]    received new command: ("start", "Test", "4") and options: ()
[2016-12-10T15:08:10 Debug]    received new command: ("Test5") and options: ("a", "i")
[2016-12-10T15:08:17 Debug]    received new command: ("Test6") and options: ("i")
[2016-12-10T15:08:38 Debug]    received new command: ("stop", "Test") and options: ("f")
```

#### Terminal 4
##### After Command
```
[Warning]  QBackgroundProcess: Master is already running. Start arguments will be passed to it as is
[2016-12-10T15:08:04 Debug]    received new command: ("start", "Test", "4") and options: ()
```

##### Completely
```
[Warning]  QBackgroundProcess: Master is already running. Start arguments will be passed to it as is
[2016-12-10T15:08:04 Debug]    received new command: ("start", "Test", "4") and options: ()
[2016-12-10T15:08:10 Debug]    received new command: ("Test5") and options: ("a", "i")
[2016-12-10T15:08:17 Debug]    received new command: ("Test6") and options: ("i")
[2016-12-10T15:08:38 Debug]    received new command: ("stop", "Test") and options: ("f")
```

#### Terminal 5
##### After Command
```
[2016-12-10T15:08:10 Debug]    received new command: ("Test5") and options: ("a", "i")
```

##### Completely
```
[2016-12-10T15:08:10 Debug]    received new command: ("Test5") and options: ("a", "i")
[2016-12-10T15:08:17 Debug]    received new command: ("Test6") and options: ("i")
[2016-12-10T15:08:38 Debug]    received new command: ("stop", "Test") and options: ("f")
```

#### Terminal 6
##### After Command
```
[2016-12-10T15:08:17 Debug]    received new command: ("Test6") and options: ("i")
```

##### Completely
```
[2016-12-10T15:08:17 Debug]    received new command: ("Test6") and options: ("i")
[2016-12-10T15:08:38 Debug]    received new command: ("stop", "Test") and options: ("f")
```

#### Terminal 7
##### After Command
```
```

##### Completely
```
[2016-12-10T15:08:38 Debug]    received new command: ("stop", "Test") and options: ("f")
```

#### Terminal 8
##### After Command
```
```

##### Completely
```
[2016-12-10T15:08:38 Debug]    received new command: ("stop", "Test") and options: ("f")
```

#### Terminal 9
```
[2016-12-10T15:08:38 Debug]    received new command: ("stop", "Test") and options: ("f")
```

#### Log File
```
[2016-12-10T15:07:26 Debug]    App Master Started with arguments: ("__qbckgrndprcss$start#master~", "Test2") and options: ("a", "f", "i")
[2016-12-10T15:07:26 Debug]    skipping starter args: ("Test2") and options: ("a", "f", "i")
[2016-12-10T15:07:56 Debug]    received new command: () and options: ()
[2016-12-10T15:08:04 Debug]    received new command: ("start", "Test", "4") and options: ()
[2016-12-10T15:08:10 Debug]    received new command: ("Test5") and options: ("a", "i")
[2016-12-10T15:08:17 Debug]    received new command: ("Test6") and options: ("i")
[2016-12-10T15:08:24 Debug]    received new command: ("Test", "7") and options: ("f")
[2016-12-10T15:08:31 Debug]    received new command: ("Test", "8") and options: ()
[2016-12-10T15:08:38 Debug]    received new command: ("stop", "Test") and options: ("f")
[2016-12-10T15:08:38 Debug]    stop requested with ("stop", "Test") and options: ("f")
```
