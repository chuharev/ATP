SECTIONS {
.text : {
    *(.text)
    *(.rdata)
    *(.CRT)
    *(.tls)
    *(SORT(.rdata$*))
    *(SORT(.CRT$*))
    *(SORT(.tls$*))
}
}
