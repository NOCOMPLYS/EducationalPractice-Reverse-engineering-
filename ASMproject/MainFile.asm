format PE Console 4.0
entry start

include '%finc%\win32\win32a.inc'
include 'Console.inc'

section '.idata' import data readable writable
        library kernel32, 'KERNEL32.DLL', \msvcrt, 'msvcrt.dll'
        import kernel32, ExitProcess, 'ExitProcess', GetModuleHandleW, 'GetModuleHandleW'
        import msvcrt, printf, 'printf', scanf, 'scanf', getch, '_getch'

section '.data' data readable writable
        specHHU_in db '%hhu', 0      ;спецификатор ввода
        specHHU_out db '%hhu ', 0
        array dd 5 dup(0)
        specS_out db '%s', 0  ;спецификатор строки
        entry_line db "Please, enter 5 unsigned decimal numbers from 0 to 255 inclusively: ", 0x0A, 0       ;0x0A = /n
        entered_numbers_line db "Entered numbers (decimal): ", 0x0A, 0
        result_line db "Result (decimal): ", 0x0A, 0
        press_any_key_line db "Press any key...", 0
        clear_line db " ", 0x0A, 0
        entered_number dd 0
        index dd 0

section '.code' code readable executable

start:
        cinvoke printf, specS_out, entry_line
        mov ebx, 0
        mov edx, 0
        for:
                cmp ebx, 0x05
                je after_input
                invoke scanf, specHHU_in, entered_number
                mov edx, [entered_number]         ;[...] = само значение, а не адрес
                mov [array + ebx], edx
                inc ebx
        jmp for

after_input:
        cinvoke printf, specS_out, clear_line
        cinvoke printf, specS_out, clear_line
        xor ebx, ebx
        xor edx, edx
        mov esi, array
        cinvoke printf, specS_out, entered_numbers_line
        cld
        print_entered_numbers:
                  cmp ebx, 5
                  je algorithm
                  lodsb
                  mov [entered_number], eax
                  cinvoke printf, specHHU_out, [entered_number]
                  inc ebx
        jmp print_entered_numbers

algorithm:
        cinvoke printf, specS_out, clear_line
        invoke GetModuleHandleW, 0
        cmp eax, 0x00
        je final
        xor edx, edx
        xor ebx, ebx
        xor ecx, ecx
        mov cl, [eax]        ;Кладём M
        mov dl, [eax +1]     ;Кладём Z
        xor eax, eax
        mov esi, array
        mov edi, array
        mov [index], 0
        cld
        jmp trans

trans:
        cmp [index], 5
        je final
        xor eax, eax
        lodsb      ;достаём элемент из массива
        xor al, dl
        and al, cl
        and al, byte[index]
        stosb      ;кладём элемент в массив
        inc [index]
        jmp trans

final:
        cinvoke printf, specS_out, clear_line
        xor ebx, ebx
        xor edx, edx
        mov esi, array
        cinvoke printf, specS_out, result_line
        cld
        print_result_numbers:
                  cmp ebx, 5
                  je exit
                  lodsb
                  mov [entered_number], eax
                  cinvoke printf, specHHU_out, [entered_number]
                  inc ebx
                  jmp print_result_numbers
exit:
        cinvoke printf, specS_out, clear_line
        cinvoke printf, specS_out, clear_line
        invoke printf, specS_out, press_any_key_line
        invoke getch
        invoke ExitProcess, 0

