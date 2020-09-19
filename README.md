![image](https://github.com/suspex0/ProxyJect/blob/master/assets/logo.png)
- Inject dll files over a proxy process in to your target process. 
```
ALPHA VERSION STILL IN DEVELOPMENT
```

### What is ProxyJect?
ProxyJect is POC (proof of concept) app for injecting dll files into a target process. The big difference to a usual injector is that dll file doesn't get injected directly into the target process. Instead, a **stub** dll will be loaded into a selectable **proxy-process** and from there the actual dll file gets injected into the target.

![image](https://github.com/suspex0/ProxyJect/blob/master/assets/proxyject.png)

### What can it be used for?
ProxyJect main usage is to inject dll files into games for gamehacking, to bypass features of common anti-cheat systems. Last but not least ProxyJect can be used 
to bypass handle access-rights restrictions and/or to inject debugging tools into malware or viruses.

#### Benefits to a normal injection:
1. The injector cant be detected by any signature based detections because its not running anymore while the real injection takes place.
2. The game can't backtrack the process handle to the real source because most traces will fall back to proxy application.
3. Depending on which proxy-process is selected it can be used to hide any suspicious or unnusual behaviours.

### How to use?
```
Log files of ProxyJect you find under C:\Users\username\AppData\Roaming\ProxyJect\
```
1. Download the latest build [here](https://github.com/suspex0/ProxyJect/releases) or compile your own version (make sure that you include the stub64.dll).
2. Modify the **ProxyJect.json** file to your needs. If that file dosen't exist, run **ProxyJect.exe** once and it will auto-generate a config for you.
(You can place "none" as veto keyword into your configuration file to replace one of the options "target-executable" or "target-window")
3. Run **ProxyJect.exe**
4. Run your "ProxyApplication"
5. Run the target process you want to inject the dll file.


#### Example video
[![ProxyJectVideo](https://raw.githubusercontent.com/suspex0/ProxyJect/master/assets/video-thumbnail.png)](https://streamable.com/b55zxq "Click to watch video.")


### Support
- Support Window 10(64bit) (Only **tested** on windows 10 version 1809 and 2004. But it should also run on windows 7 and higher **untested**)
- Currently only supports **64bit** proxy-process & dll files. (**x86 support will be added soon**)

### Special features
- Proxy "stub.dll" file has string encryption to prevent any string based detection vectors
- OpenProcess function is manually imported to hide windows-api call
- Manually write Dll into target process and resolve imports of it (not using LoadLibary or any similar windows-api calls)

### Credits
- fmt lib [github.com/fmtlib/fmt](https://github.com/fmtlib/fmt)
- fleep
- Broihon
- skCrypt [github.com/skadro-official/skCrypter](https://github.com/skadro-official/skCrypter)
- XorString by LeFF

### To-do list

```
- Add x86 support for the dll and proxy
- Add handle hijack from proxy if a handle exist to target
- Add encrypt loader and proxy communication
- Add unlink proxy module
- Add some generic options to randomize the proxy and loader 
- Add update checker
- Add multiple injection methods with random auto selection option
- Add option for multiple process that can be used as proxy and a random auto selection for current running applications
- Add a more "hidden" way to communicate between loader and proxy process
- Add a real GUI for the loader to setup the settings.
- and more..
```
