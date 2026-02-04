[![Win](https://github.com/aiekick/EzLibs/actions/workflows/Win.yml/badge.svg)](https://github.com/aiekick/EzLibs/actions/workflows/Win.yml)
[![Linux](https://github.com/aiekick/EzLibs/actions/workflows/Linux.yml/badge.svg)](https://github.com/aiekick/EzLibs/actions/workflows/Linux.yml)
[![Code Coverage](https://github.com/aiekick/ezLibs/actions/workflows/Coverage.yml/badge.svg)](https://aiekick.github.io/ezLibs)
[![CodeFactor](https://www.codefactor.io/repository/github/aiekick/ezlibs/badge/master)](https://www.codefactor.io/repository/github/aiekick/ezlibs/overview/master)

# EzLibs

its my collection of Cpp11 cross platform self made "one header file only" libs

# dependencies 

No External libraries dependencies needed except Cpp11 standard library

Just the lib [EzGL](doc/EzGL.md) is depending of a Opengl Loader like [Glad](https://github.com/Dav1dde/glad/tree/c)

# HowTo 

you just need to include the libs in your project and use it. thats all.

Some libs are in inter dependencies, so you may need to include the EzLibs dir in your building system

# Available Libs :

|Lib|Doc|Tested|Status|Description|
|-|-|-|-|-|
|[ezAABB](doc/ezAABB.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|2D AABB Manipulation|
|[ezAABBCC](doc/ezAABBCC.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|3D AABBCC Manipulation|
|[ezActions](doc/ezActions.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|ezActions|
|[ezApp](doc/ezApp.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|App Manipulation, Current working dir|
|[ezArgs](doc/ezArgs.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|Command Options parsing|
|[ezBinBuf](doc/ezBinBuf.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|ezBinBuf|
|[ezBmp](doc/ezBmp.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|Bitmap WriterzBmp|
|[ezBuildInc](doc/ezBuildInc.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|ezBuildInc|
|[ezClass](doc/ezClass.md)|:construction:|:no_entry:|:construction:|ezClass|
|[ezCmdProc](doc/ezCmdProc.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|ezCmdProc|
|[ezCnt](doc/ezCnt.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|Spetials Containers|
|[ezCron](doc/ezCron.md)|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|Crontab parsing and time execution|
|[ezCsv](doc/ezCsv.md)|:construction:|:no_entry:|:construction:|Csv manipulation|
|[ezCTest](doc/ezCTest.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|ezCTest|
|[ezDate](doc/ezDate.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|ezDate|
|[ezEmbed](doc/ezEmbed.md)|:construction:|:no_entry:|:construction:|ezEmbed|
|[ezExpr](doc/ezExpr.md)|:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|Math expression parsing and computing|
|[ezFdGraph](doc/ezFdGraph.md)|:construction:|:no_entry:|:construction:|ezFdGraph|
|[ezFigFont](doc/ezFigFont.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|ezFigFont|
|[ezFile](doc/ezFile.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|File Manipulation|
|[ezFmt](doc/ezFmt.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|ezFmt|
|[ezGraph](doc/ezGraph.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|Internal Node Graph System|
|[ezImGui](doc/ezImGui.md)|:construction:|:no_entry:|:construction:|ezImGui|
|[ezIni](doc/ezIni.md)|:construction:|:no_entry:|:construction:|Ini File Reader/Writer|
|[ezLog](doc/ezLog.md)|:construction:|:construction:|:construction:|Log File Writer|
|[ezMat2](doc/ezMat2.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|Mat2 manipulation|
|[ezMat3](doc/ezMat3.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|Mat3 manipulation|
|[ezMat4](doc/ezMat4.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|Mat4 manipulation|
|[ezMath](doc/ezMath.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|Math methods|
|[ezNamedPipe](doc/ezNamedPipe.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|ezNamedPipe|
|[ezNet](doc/ezNet.md)|:construction:|:no_entry:|:construction:|ezNet|
|[ezGL](doc/ezGL.md)|:construction:|:heavy_check_mark:|:construction:|Opengl manipulation|
|[ezGeo](doc/ezGeo.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|Geo manipulation|
|[ezOS](doc/ezOS.md)|:construction:|:no_entry:|:construction:|ezOS|
|[ezPlane](doc/ezPlane.md)|:construction:|:no_entry:|:construction:|3D Plane Manipulation|
|[ezPlugin](doc/ezPlugin.md)|:construction:|:no_entry:|:construction:|ezPlugin|
|[ezQuadTree](doc/ezQuadTree.md)|:construction:|:no_entry:|:construction:|ezQuadTree|
|[ezQuat](doc/ezQuat.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|Quaternion Manipulation|
|[ezScreen](doc/ezScreen.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|ezScreen|
|[ezSerial](doc/ezSerial.md)|:construction:|:no_entry:|:construction:|ezSerial|
|[ezSha](doc/ezSha.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|ezSha|
|[ezSingleton](doc/ezSingleton.md)|:construction:|:no_entry:|:construction:|ezSingleton|
|[ezSqlite](doc/ezSqlite.md)|:construction:|:construction:|:construction:|ezSqlite|
|[ezStackString](doc/ezStackString.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|ezStackString|
|[ezStr](doc/ezStr.md)|:construction:|:no_entry:|:construction:|String Manipulation|
|[ezTemplater](doc/ezTemplater.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|ezTemplater|
|[ezTile](doc/ezTile.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|Geo Tile manipulation|
|[ezTime](doc/ezTime.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|Time manipulation|
|[ezTools](doc/ezTools.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|ezTools|
|[ezVariant](doc/ezVariant.md)|:construction:|:no_entry:|:construction:|Variant/Conversion for EzLIbs compatible types|
|[ezVdbWriter](doc/ezVdbWriter.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|OpenVdb partial file format writer|
|[ezVec2](doc/ezVec2.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|Vec2 Manipulation|
|[ezVec3](doc/ezVec3.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|Vec3 Manipulation|
|[ezVec4](doc/ezVec4.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|Vec4 Manipulation|
|[ezVoxWriter](doc/ezVoxWriter.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|Magicavoxel partial file format writer|
|[ezWorkerThread](doc/ezWorkerThread.md)|:construction:|:no_entry:|:construction:|ezWorkerThread|
|[ezXml](doc/ezXml.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|xml Parsing|
|[ezXmlConfig](doc/ezXmlConfig.md)|:construction:|:heavy_check_mark:|:heavy_check_mark:|xml config File Reader/Writer|

# License

Copyright :copyright: 2014-2024 Stephane Cuillerdier

See the [License file](LICENSE)
