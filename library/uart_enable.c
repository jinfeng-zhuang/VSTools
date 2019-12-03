
int uart_enable(int enable)
{
    if (enable)
        RegWrite8(0x1500ee1d, 0x11);
    else
        RegWrite8(0x1500ee1d, 0x00);
}

