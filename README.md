# Direct Composition Hook Writeup

### What Is DirectComposition?
DirectComposition is typically used by native windows applications to render bitmap graphics and is seldom used by games. Its less of a graphics pipeline and more of a composition engine that moves around and animates already rendered scenes from a graphics pipeline like DirectX or Direct2D. It is referred to as a bitmap composition engine by Microsoft.

### We Can't Use DirectX Hooks
IMAGE1
![Screenshot_1](https://raw.githubusercontent.com/dognews/DirectCompositionHook/refs/heads/main/resources/Image1.png)<br/>
Using a DirectX hook would be more conventional, as when running your cheat in the hook we could also read memory from the game without an additional context switch, and synchronize perfectly with the frame-rate of the game. However, DirectX function calls in modern windows versions are no longer routed through data pointers in win32k, meaning DirectX hooks are not feasible.

### We Can Use DirectComposition Hooks
![Screenshot_2](https://raw.githubusercontent.com/dognews/DirectCompositionHook/refs/heads/main/resources/Image2.png)<br/>
DirectComposition functions are still routed through win32k, meaning we can place hooks on functions involved in this bitmap composition engine. Even though DirectComposition isn’t directly involved in scene rendering it still manipulates already rendered scenes and this is why certain functions (like Present) are synchronized with the monitors’ refresh rate 

### DirectComposition Hook Issues
DirectComposition hooks do not sync perfectly with the refresh rate of the game. Games may render frames faster or slower than the refresh rate of the monitor, depending on GPU load and game settings. The main goal here is the synchronization of kernel drawing, which does occur when hooking DirectComposition's present function. 

### Addressing DirectComposition Hook Issues
We can prevent the cheat from running slower or faster than the game by turning on VSYNC, or limiting the refresh rate of the game to that of the monitor. This will ensure that DirectComposition refreshes at the same rate as the game. It’s frustrating that games don’t actually use DirectComposition, so we cant just read their memory when they call a DirectComposition function. Despite this, it’s still very trivial to read memory, we can just attach to the process using KeStackAttachProcess then read game memory. Detection vectors aside (for kernel anticheats), this is a very efficient process and only involves a single context switch per frame to read memory.

### How Does This Work?
![Screenshot_3](https://raw.githubusercontent.com/dognews/DirectCompositionHook/refs/heads/main/resources/Image3.png)<br/>
I am mainly looking at this from the perspective of the kernel, and not through any user mode lens. I found this function searching through NT functions in win32k that looked to be related to a graphics API of some sort and would only be called once per frame. I started with DirectX then realized DirectX functions were no longer routed through win32k. Instead I focused on alternative rendering functions and found a whole set of DirectComposition functions. I tested several of these functions, and realized the ones that are called consistently were called by multiple processes using DirectComposition. I created a test driver that hooked all of the functions related to DirectComposition (NtDComposition functions) and recorded the number of calls that occurred in a second. I added a guard that only let the counter increment when one process calls (DWM Process). I didn't test every function all at once. I went in a sequential order and NtDCompositionBeginFrame was the first to meet my criteria. There is likely other DirectComposition functions that do the same thing if you want to experiment with this.

### How Can We Use This In A Cheat
Using this in a cheat is simple, this repository is a hacked together POC that builds on this hook idea. It uses a set of GDI functions to draw a rectangle to the screen, and hooks another function it uses for communication with usermode. If this were to be used in a real cheat, the most practical setup would be for the entire cheat to run in a hook. All memory reading, calculations, and drawing would occur every time the function is called. Settings could be changed using another hooked function to communicate with user mode (like in this POC) that changes variables stored in a pool of memory that the hook can also access.

### Usermode Anticheat Detectability
Its difficult to draw against an effective user mode anti-cheat. They detect any topmost windows and aren’t going to let us draw to the main screen DC or some DC above the game. They also aren’t going to let us touch their window for drawing. DirectX hooks for internals have to be creative as many areas related to DirectX like the VMT are watched for changes to memory. The most effective solution I’ve seen is hijacking a legitimate processes topmost overlay to draw over the game. However, a really aggressive user mode anti-cheat might just block any attempts an application makes (legitimate or not) to render a window on top of it. I’ve seen kernel drawing methods but most hookless methods are hacky, often trying to draw every millisecond which burns CPU cycles and creates flickering. Most hook-based methods are outdated and no longer work on recent windows versions.

### Kernel Mode Anticheat Detectability
This in its current state is not effective against any kernel mode anti-cheat. For this to be effective against kernel mode anti-cheats you could start by having your driver being signed. If a signed driver did this you might not get in trouble with stack walking, as the hook is stored in a legitimate memory region. The anticheat may not like it if the hook isn’t stored in the module it expects the pointer to point to (win32kfull, win32kbase). If pointers are manually checked by a kernel anti-cheat then you are doomed. Reading the memory the way i described it also isn't safe for these ACs, you would be better off reading physical memory and not attaching to the process.
