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
[2016-12-09T20:06:33 Debug]    App Master Started with arguments: ("Test1")
[2016-12-09T20:06:33 Debug]    skipping starter args: ("start", "Test1")
[2016-12-09T20:07:03 Debug]    received new command: ("Test2")
[2016-12-09T20:07:12 Debug]    received new command: ("start", "Test", "3")
[2016-12-09T20:07:21 Debug]    received new command: ("stop", "Test", "4")
[2016-12-09T20:07:21 Debug]    stop requested with ("stop", "Test", "4")
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
[2016-12-09T20:27:05 Debug]    App Master Started with arguments: ("-a", "-f", "1", "-i", "Test2")
[2016-12-09T20:27:05 Debug]    skipping starter args: ("-a", "-f", "1", "-i", "Test2")
```

##### Completely
```
[2016-12-09T20:27:05 Debug]    App Master Started with arguments: ("-a", "-f", "1", "-i", "Test2")
[2016-12-09T20:27:05 Debug]    skipping starter args: ("-a", "-f", "1", "-i", "Test2")
[2016-12-09T20:27:16 Debug]    received new command: ()
[2016-12-09T20:27:28 Debug]    received new command: ("start", "Test", "4")
[2016-12-09T20:27:38 Debug]    received new command: ("-a", "-i", "Test5")
[2016-12-09T20:27:48 Debug]    received new command: ("-i", "Test6")
[2016-12-09T20:28:20 Debug]    received new command: ("stop", "-f", "1", "Test9")
```

#### Terminal 3
##### After Command
```
[Warning]  QBackgroundProcess: Start commands ignored because master is already running! The terminal will connect with an empty argument list!
[2016-12-09T20:27:16 Debug]    received new command: ()
```

##### Completely
```
[Warning]  QBackgroundProcess: Start commands ignored because master is already running! The terminal will connect with an empty argument list!
[2016-12-09T20:27:16 Debug]    received new command: ()
[2016-12-09T20:27:28 Debug]    received new command: ("start", "Test", "4")
[2016-12-09T20:27:38 Debug]    received new command: ("-a", "-i", "Test5")
[2016-12-09T20:27:48 Debug]    received new command: ("-i", "Test6")
[2016-12-09T20:28:20 Debug]    received new command: ("stop", "-f", "1", "Test9")
```

#### Terminal 4
##### After Command
```
[Warning]  QBackgroundProcess: Master is already running. Start arguments will be passed to it as is
[2016-12-09T20:27:28 Debug]    received new command: ("start", "Test", "4")
```

##### Completely
```
[Warning]  QBackgroundProcess: Master is already running. Start arguments will be passed to it as is
[2016-12-09T20:27:28 Debug]    received new command: ("start", "Test", "4")
[2016-12-09T20:27:38 Debug]    received new command: ("-a", "-i", "Test5")
[2016-12-09T20:27:48 Debug]    received new command: ("-i", "Test6")
[2016-12-09T20:28:20 Debug]    received new command: ("stop", "-f", "1", "Test9")
[2016-12-09T20:28:20 Debug]    stop requested with ("stop", "-f", "1", "Test9")
```

#### Terminal 5
##### After Command
```
[2016-12-09T20:27:38 Debug]    received new command: ("-a", "-i", "Test5")
```

##### Completely
```
[2016-12-09T20:27:38 Debug]    received new command: ("-a", "-i", "Test5")
[2016-12-09T20:27:48 Debug]    received new command: ("-i", "Test6")
[2016-12-09T20:28:20 Debug]    received new command: ("stop", "-f", "1", "Test9")
```

#### Terminal 6
##### After Command
```
[2016-12-09T20:27:48 Debug]    received new command: ("-i", "Test6")
```

##### Completely
```
[2016-12-09T20:27:48 Debug]    received new command: ("-i", "Test6")
[2016-12-09T20:28:20 Debug]    received new command: ("stop", "-f", "1", "Test9")
[2016-12-09T20:28:20 Debug]    stop requested with ("stop", "-f", "1", "Test9")
```

#### Terminal 7
##### After Command
```
```

##### Completely
```
[2016-12-09T20:28:20 Debug]    received new command: ("stop", "-f", "1", "Test9")
[2016-12-09T20:28:20 Debug]    stop requested with ("stop", "-f", "1", "Test9")
```

#### Terminal 8
##### After Command
```
```

##### Completely
```
[2016-12-09T20:28:20 Debug]    received new command: ("stop", "-f", "1", "Test9")
[2016-12-09T20:28:20 Debug]    stop requested with ("stop", "-f", "1", "Test9")
```

#### Terminal 9
```
[2016-12-09T20:28:20 Debug]    received new command: ("stop", "-f", "1", "Test9")
[2016-12-09T20:28:20 Debug]    stop requested with ("stop", "-f", "1", "Test9")
```

#### Log File
```
[2016-12-09T20:27:05 Debug]    App Master Started with arguments: ("-a", "-f", "1", "-i", "Test2")
[2016-12-09T20:27:05 Debug]    skipping starter args: ("-a", "-f", "1", "-i", "Test2")
[2016-12-09T20:27:16 Debug]    received new command: ()
[2016-12-09T20:27:28 Debug]    received new command: ("start", "Test", "4")
[2016-12-09T20:27:38 Debug]    received new command: ("-a", "-i", "Test5")
[2016-12-09T20:27:48 Debug]    received new command: ("-i", "Test6")
[2016-12-09T20:28:02 Debug]    received new command: ("-f", "0", "Test", "7")
[2016-12-09T20:28:12 Debug]    received new command: ("Test", "8")
[2016-12-09T20:28:20 Debug]    received new command: ("stop", "-f", "1", "Test9")
[2016-12-09T20:28:20 Debug]    stop requested with ("stop", "-f", "1", "Test9")

```
