#ifndef VS_DEMUX_H
#define VS_DEMUX_H

typedef struct {
    volatile unsigned int Reserved0;                  /* 0x00 reset vector 16 bits, irq vector 16 bits */
    volatile unsigned int Reserved1;                  /* 0x04 break point vector 16 bits, Reserved 16 bits */
    volatile unsigned int Reserved2;                  /* 0x08 Reserved for security */
    volatile unsigned int Reserved3;                  /* 0x0c */
    volatile unsigned int Reserved4;                  /* 0x10 */
    volatile unsigned int Reserved5;                  /* 0x14 */
    volatile unsigned int ParserCtrl;                 /* 0x18 Parser Control */
    volatile unsigned int VersionNumber;              /* 0x1c Version Number */
    volatile unsigned int ParserStatus;               /* 0x20 Parser Status */
    volatile unsigned int Capabilities;               /* 0x24 */
    volatile unsigned int InternalErrorCode;          /* 0x28 TSP unit internal error code */
    volatile unsigned int MsgFilter;                  /* 0x2c Message 0 - 31, messages not sent to host */
    volatile unsigned int Reserved6;                  /* 0x30  */
    volatile unsigned int CmdInterfaceDescriptor;     /* 0x34 Command Interface descriptor */
    volatile unsigned int MsgInterfaceDescriptor;     /* 0x38 Message Inteferface descriptor */
    volatile unsigned int PidTabDescriptor;           /* 0x3c PID table descriptor */
    volatile unsigned int KeyTabDescriptor;           /* 0x40 Key Table descriptor */
    volatile unsigned int PsiBufTabDescriptor;        /* 0x44 PSI buffer descriptor */
    volatile unsigned int XprtBufTabDescriptor;       /* 0x48 Transport Table buffer descriptor */
    volatile unsigned int EventLogBufTabDescriptor;   /* 0x4c Event Log Buffer descriptor */
    volatile unsigned int FilterTabDescriptor;        /* 0x50 Filter Table Descriptor */
    volatile unsigned int CaTabDescriptor;            /* 0x54 2 types of Desc if using NDS or Multi2 */
    volatile unsigned int EsBufferDescriptor;         /* 0x58 ES buffer descriptor */
    volatile unsigned int PTSDescriptor;              /* 0x5c  PTS buffer Descriptor*/
    volatile unsigned int PvrTabDescriptor;           /* 0x60 PVR Table Descriptor */
    volatile unsigned int XprtStreamTimeout;          /* 0x64 Transport Stream Timeout */
    volatile unsigned int DescramblerControl;         /* 0x68 Descrambler control */
    volatile unsigned int IdleCount;                  /* 0x6c Idle cycle count */
    volatile unsigned int StreamTabDescriptor;        /* 0x70 Pecos only */
    volatile unsigned int FilteringTabDescriptor;     /* 0x74 Filtering Table Descriptor */
    volatile unsigned int IndexingTabDescriptor;      /* 0x78 Indexing Table Descriptor */
    volatile unsigned int AuxPidTabDescriptor;        /* 0x7c Auxiliary Table Descriptor */
    volatile unsigned int Reserved32;                 /* 0x80  */
    volatile unsigned int PacketNumber;               /* 0x84 Packet Number */
    volatile unsigned int WaterTabDescriptor;         /* 0x88 Watermarking Table Descriptor*/
    volatile unsigned int WaterEventBufTabDescriptor; /* 0x8c Watermarking Event Buffer Table Descriptor*/
    volatile unsigned int AudioDataTabDescriptor;     /* 0x90 Audio Data Table Descriptor*/
    volatile unsigned int STCIndexDescriptor;         /* 0x94 STC Index descriptor */
    volatile unsigned int PCRtabDescriptor;           /* 0x98 PCR Table Descriptor */
    volatile unsigned int TestVectorDescriptor;       /* 0x9c Test Vector Address */
    volatile unsigned int CodeSignature;              /* 0xa0 Code Signature */
    volatile unsigned int SecTabDescriptor;           /* 0xa4 Security Table Descriptor */
} DMX_DESCRIPTOR;

typedef struct {
    volatile unsigned int uWritePtr;
    volatile unsigned int uReadPtr;
    volatile unsigned int uBufStartPtr;
    volatile unsigned int uBufEndPtr;
} DMX_PTS_BUF_DESCRIPTOR; 

#endif
