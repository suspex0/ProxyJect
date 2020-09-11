# ProxyJect
"LOGO HERE"
Inject Dll over a proxy process in to your target process. 

### What is ProxyJect ?
ProxyJect is POC (proof of concept) app for injecting dll files in to are target process. The big difference to a usual injector is that dll file dont get injected directly in to the target process, instead a **stub** dll will be loaded in to a selectable **proxy-process** and from there the actual dll file gets injected in to the target.

"IMAGE HERE"

### What it can be used for ?
ProxyJect main usage is to inject dll files in to games for gamehacking.
The benefit to a normal injection is that:
1. The injector cant be detected by any signature based detections.
2. The game cant backtrack the process handle to the real source.
3. Depending on which proxy-process is selected can be used to hide any suspicious activitys.

Last but not least ProxyJect can be used to elevate handle access and/or to inject debugging tools in to malware/viruses.

### How to use ?
1. Download the latest build [here](https://github.com/suspex0/ProxyJect/releases/tag/ProxyJect-alpha-1.0) or compile your own version. ( make sure to include the stub64.dll )
2. Modify the **ProxyJect.json** file to your needs. If thefile dont exist run **ProxyJect.exe** once and it will auto-generate a config for you.
(You can place "none" in to config target-executable or the target-window as a veto keyword.)
3. Run **ProxyJect.exe**
4. Run your "ProxyApplication"
5. Run the target process to inject to.

### Support
- Support Window 7-10(64bit) ( only tested on windows 10 version 1809 and 2004 )
- Currently only supports 64bit proxy-process & dll files. ( x86 support will be added soon )

### To-do list
- Fix disable_log for stub64
- Fix show_console for stub64
- Add x86 support the dll and proxy
- Add store & load dll from memory ( to hide source )
- Add auto-updater
- Add multiple injection methods
- Add a more "hidden" way to communicate between loader and proxy process
