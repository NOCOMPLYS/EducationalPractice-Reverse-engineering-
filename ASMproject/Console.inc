
; Console Modes, and color atributes

ENABLE_PROCESSED_INPUT          = 1h
ENABLE_LINE_INPUT               = 2h
ENABLE_ECHO_INPUT               = 4h
ENABLE_WINDOW_INPUT             = 8h
ENABLE_MOUSE_INPUT              = 10h
ENABLE_PROCESSED_OUTPUT         = 1h
ENABLE_WRAP_AT_EOL_OUTPUT       = 2h
CONSOLE_TEXTMODE_BUFFER         = 1
FOREGROUND_BLUE                 = 1h
FOREGROUND_GREEN                = 2h
FOREGROUND_RED                  = 4h
FOREGROUND_INTENSITY            = 8h
BACKGROUND_BLUE                 = 10h
BACKGROUND_GREEN                = 20h
BACKGROUND_RED                  = 40h
BACKGROUND_INTENSITY            = 80h

struct COORD
  .x                            dw ?
  .y                            dw ?
ends

struct SMALL_RECT
  .Left                         dw ?
  .Top                          dw ?
  .Right                        dw ?
  .Bottom                       dw ?
ends

struct CONSOLE_SCREEN_BUFFER_INFO
  .dwSize                       COORD
  .dwCursorPosition             COORD
  .wAttributes                  dw ?
  .srWindow                     SMALL_RECT
  .dwMaximumWindowSize          COORD
ends

; Console Colors

macro ConsoleColor Name,[Part] {
 common
   local Color
   Color = 0
 forward
   if ~ Part eq
     Color = Color or FOREGROUND_#Part
   end if
 common
   FGColor.#Name = Color
   BGColor.#Name = (Color shl 4)
}

ConsoleColor Black
ConsoleColor Gray,INTENSITY
ConsoleColor Silver,RED,GREEN,BLUE
ConsoleColor White,RED,GREEN,BLUE,INTENSITY
ConsoleColor Maroon,RED
ConsoleColor Red,RED,INTENSITY
ConsoleColor Navy,BLUE
ConsoleColor Blue,BLUE,INTENSITY
ConsoleColor Green,GREEN
ConsoleColor Lime,GREEN,INTENSITY
ConsoleColor Purple,RED,BLUE
ConsoleColor Pink,RED,BLUE,INTENSITY
ConsoleColor Olive,RED,GREEN
ConsoleColor Yellow,RED,GREEN,INTENSITY
ConsoleColor Teal,BLUE,GREEN
ConsoleColor Cyan,BLUE,GREEN,INTENSITY

; Console Procedures

iglobal
  CSBI          CONSOLE_SCREEN_BUFFER_INFO
  coords        COORD
endg

uglobal
  hStdIn        dd ?
  hStdOut       dd ?
  ConsoleBuffer rb 1024
endg

; FUNCTION SetColor (Color)
; Changes the Console color.
Console.SetColor fix [SetConsoleTextAttribute],[hStdOut]

initialize PrepareConsole
begin
    xor         eax,eax
    mov         dword [coords],eax

    invoke      GetStdHandle,STD_INPUT_HANDLE
    cmp         eax,INVALID_HANDLE_VALUE ;If theres an error, dont open the console
    je          .end
    mov         [hStdIn],eax ;else, save the input handle
    invoke      GetStdHandle,STD_OUTPUT_HANDLE
    cmp         eax,INVALID_HANDLE_VALUE ;If theres an error, dont open the console
    je          .end
    mov         [hStdOut],eax ;else save the output handle
    invoke      SetConsoleMode,[hStdIn],ENABLE_PROCESSED_INPUT or ENABLE_LINE_INPUT or ENABLE_ECHO_INPUT or \
                                                ENABLE_MOUSE_INPUT or ENABLE_WINDOW_INPUT or ENABLE_PROCESSED_OUTPUT or \
                                                ENABLE_WRAP_AT_EOL_OUTPUT
    stdcall     Console.SetColor,FGColor.Silver or BGColor.Black
  .end:
    return
endp

; FUNCTION Print (Text)
; Print a text to the console.
proc Console.Print,.strText
  .WB dd ?
begin
     push       edx
     invoke     lstrlen,[.strText]
     lea        edx,[.WB]
     invoke     WriteConsoleA, [hStdOut], [.strText], eax, edx, 0
     pop        edx
.end:
     return
endp


; FUNCTION Input
; Read one text from the console.
; Returns pointer to buffer.
proc Console.Input
  .RB dd ?
begin
     invoke     FlushConsoleInputBuffer,[hStdIn]
     lea        eax,[.RB]
     invoke     ReadConsoleA,[hStdIn],ConsoleBuffer,1024,eax,0
     mov        esi,ConsoleBuffer ; starting the routine to remove the crlf chars
.loadstrloop:
     lodsb
     cmp        al,13
     jne        .loadstrloop
     mov        byte [esi-1],0
     mov        eax,ConsoleBuffer
     return
endp

; FUNCTION Locate (X,Y)
; Changes the cursor position.
proc Console.Locate,.curX,.curY
begin
     mov        eax,[.curX]
     mov        dword [coords.x],eax
     mov        eax,[.curY]
     mov        dword [coords.y],eax
     invoke     SetConsoleCursorPosition,[hStdOut],[coords]
     return
endp

; FUNCTION Clear (Color)
; Clears the console.
proc Console.Clear,.paramColor
  .WB dd ?
begin
     mov        [coords.x],0
     mov        [coords.y],0
     stdcall    Console.SetColor,[.paramColor]
.next:
     invoke     GetConsoleScreenBufferInfo,[hStdOut],CSBI
     mov        eax,dword [CSBI.dwSize.x]
     mul        [CSBI.dwSize.y]
     ; conSize = edx
     lea        eax,[.WB]
     invoke     FillConsoleOutputCharacterA,[hStdOut],20h,edx,[coords],eax
     invoke     GetConsoleScreenBufferInfo,[hStdOut],CSBI
     lea        eax,[.WB]
     invoke     FillConsoleOutputAttribute,[hStdOut],dword [CSBI.wAttributes],edx,[coords],eax
     invoke     SetConsoleCursorPosition,[hStdOut],[coords]
     return
endp

; FUNCTION SetTitle (Title)
; Changes the console title.
Console.SetTitle equ [SetConsoleTitleA]