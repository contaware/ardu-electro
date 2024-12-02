For Windows 7 installation copy renesas_unor4minima.inf and 
renesas_unor4wifi.inf and remove the existing renesas.inf
(which is conflicting with Uno R4 Minima Hardware ID):
"%LOCALAPPDATA%\Arduino15\packages\arduino\hardware\renesas_uno\1.0.2\drivers\prewin10"

Now if already installed, but not working, uninstall the driver from
Device Manager by ticking "Delete the driver software for this device"
and then (re-)install it.
