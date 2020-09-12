# ProxyJect
## ALPHA VERSION STILL IN DEVELOPMENT
"LOGO HERE"
Inject Dll over a proxy process in to your target process. 

### What is ProxyJect ?
ProxyJect is POC (proof of concept) app for injecting dll files in to are target process. The big difference to a usual injector is that dll file dont get injected directly in to the target process, instead a **stub** dll will be loaded in to a selectable **proxy-process** and from there the actual dll file gets injected in to the target.

![image](https://github.com/suspex0/ProxyJect/blob/master/assets/proxyject.png)

### What it can be used for ?
ProxyJect main usage is to inject dll files in to games for gamehacking.
By bypassing features of common anti-cheats systems. Last but not least ProxyJect can be used 
to bypass handle access-rights restrictions and/or to inject debugging tools in to malware or viruses.

#### Examples for benefits to a normal injection:
1. The injector cant be detected by any signature based detections because its not running anymore while the real injection takes place.
2. The game cant backtrack the process handle to the real source because most traces will fall back to proxy application.
3. Depending on which proxy-process is selected can be used to hide any suspicious or unnusual behaviours.

### How to use ?
1. Download the latest build [here](https://github.com/suspex0/ProxyJect/releases/tag/ProxyJect-alpha-1.0) or compile your own version. ( make sure to include the stub64.dll )
2. Modify the **ProxyJect.json** file to your needs. If that file dont exist run **ProxyJect.exe** once and it will auto-generate a config for you.
(You can place "none" in to config target-executable or the target-window as a veto keyword.)
3. Run **ProxyJect.exe**
4. Run your "ProxyApplication"
5. Run the target process to inject to.

#### Example video
[![ProxyJectVideo](https://raw.githubusercontent.com/suspex0/ProxyJect/master/assets/video-thumbnail.png)](https://streamable.com/b55zxq "Click to watch video.")


### Support
- Support Window 10(64bit) ( Only **tested** on windows 10 version 1809 and 2004. But it should also run on windows 7 and higher **untested**))
- Currently only supports **64bit** proxy-process & dll files. ( **x86 support will be added soon** )

### Special features
- Proxy stub dll file has string encryption to prevent any string based detection vectors
- OpenProcess function is manualy imported to hide windows-api call

### Credits
fmt lib [github.com/fmtlib/fmt](https://github.com/fmtlib/fmt)
fleep


### To-do list
~ will be worked on in this order :wink: ~
- Fix disable_log for stub64.
- Fix show_console for stub64.
- Add x86 support the dll and proxy.
- Add encrypt loader and proxy communication.
- Add unlink proxy module.
- Add some generic options to randomize the proxy and loader.
- Add update checker.
- Add multiple injection methods with random auto selection option.
- Add option for multiple process that can be used as proxy and a random auto selection for current running applications.
- Add a more "hidden" way to communicate between loader and proxy process.
- Add a real GUI for the loader to setup the settings.
