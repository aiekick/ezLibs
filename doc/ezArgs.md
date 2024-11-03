# EzArgs

EzArgs is a one header file only for parse 
command option of an app

# how to use

```cpp
	ez::Args args;
	args.addBoolOption("-a/--auto", "automatic");    
    args.addIntOption("-p/--port", "server port", 80);  
    args.addStringOption("-l/--log", "logger file", "main.log");   
    args.addHelpOption("Usage : app [-a/--auot] [-p/--port <number>];
	args.parseOptions(argc, argv); 
	
	if (args.getBoolValue("a")) {
		// automatic
	}
	
	auto server_port = args.getIntValeu("p");
	// start server on port 
	
	auto log_file = args.getStringValue("log");
	// wrtie to log 
```


