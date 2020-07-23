.data
x200 1      ; start index
x204 10     ; stop index (not included)
x208 0      ; location to store subtraction math
x20C 0      ; sum (result)

.text
;begin:          ; have not implemented this yet!
CP x208 x200
SUB x208 x204
CMP x208
;JZ end         ; have not implemented this yet!
JZ 76
ADD x20C x200
INCR x200
;JMP begin
JMP 8

;end:            ; have not implemented this yet!
BREAK
