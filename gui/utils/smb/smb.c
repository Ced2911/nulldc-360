/*
  Copyright 2009-2010, jimmikaelkael
  Licenced under Academic Free License version 3.0
 */

#include <stdio.h>
#include <xetypes.h>
#include <lwip/sockets.h>
#include <threads/threads.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <byteswap.h>
#include "smb.h"
#include "auth.h"
#include "poll.h"
#include <debug.h>

static inline uint64_t bswap_64(int64_t b) {
	return __builtin_bswap64(b);
}

#define SMB_MAGIC	0xFF534D42

struct SMBHeader_t { //size = 36
	u32 sessionHeader;
	u32 Magic;
	u8 Cmd;
	short Eclass;
	short Ecode;
	u8 Flags;
	u16 Flags2;
	u8 Extra[12];
	u16 TID;
	u16 PID;
	u16 UID;
	u16 MID;
} __attribute__((packed));

struct SMBTransactionRequest_t { //size = 28
	u16 TotalParamCount;
	u16 TotalDataCount;
	u16 MaxParamCount;
	u16 MaxDataCount;
	u8 MaxSetupCount;
	u8 Reserved;
	u16 Flags;
	u32 Timeout;
	u16 Reserved2;
	u16 ParamCount;
	u16 ParamOffset;
	u16 DataCount;
	u16 DataOffset;
	u8 SetupCount;
	u8 Reserved3;
} __attribute__((packed));

struct SMBTransactionResponse_t { //size = 20
	u16 TotalParamCount;
	u16 TotalDataCount;
	u16 Reserved;
	u16 ParamCount;
	u16 ParamOffset;
	u16 ParamDisplacement;
	u16 DataCount;
	u16 DataOffset;
	u16 DataDisplacement;
	u8 SetupCount;
	u8 Reserved2;
} __attribute__((packed));

struct NegociateProtocolRequest_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u16 ByteCount; // 37
	u8 DialectFormat; // 39
	char DialectName[0]; // 40
} __attribute__((packed));

struct NegociateProtocolResponse_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u16 DialectIndex; // 37
	u8 SecurityMode; // 39
	u16 MaxMpxCount; // 40
	u16 MaxVC; // 42
	u32 MaxBufferSize; // 44
	u32 MaxRawBuffer; // 48
	u32 SessionKey; // 52
	u32 Capabilities; // 56
	s64 SystemTime; // 60
	u16 ServerTimeZone; // 68
	u8 KeyLength; // 70
	u16 ByteCount; // 71
	u8 ByteField[0]; // 73
} __attribute__((packed));

struct SessionSetupAndXRequest_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u8 smbAndxCmd; // 37
	u8 smbAndxReserved; // 38
	u16 smbAndxOffset; // 39
	u16 MaxBufferSize; // 41
	u16 MaxMpxCount; // 43
	u16 VCNumber; // 45
	u32 SessionKey; // 47
	u16 AnsiPasswordLength; // 51
	u16 UnicodePasswordLength; // 53
	u32 reserved; // 55
	u32 Capabilities; // 59
	u16 ByteCount; // 63
	u8 ByteField[0]; // 65
} __attribute__((packed));

struct SessionSetupAndXResponse_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u8 smbAndxCmd; // 37
	u8 smbAndxReserved; // 38
	u16 smbAndxOffset; // 39
	u16 Action; // 41
	u16 ByteCount; // 43
} __attribute__((packed));

struct TreeConnectAndXRequest_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u8 smbAndxCmd; // 37
	u8 smbAndxReserved; // 38
	u16 smbAndxOffset; // 39
	u16 Flags; // 41
	u16 PasswordLength; // 43
	u16 ByteCount; // 45
	u8 ByteField[0]; // 47
} __attribute__((packed));

struct TreeConnectAndXResponse_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u8 smbAndxCmd; // 37
	u8 smbAndxReserved; // 38
	u16 smbAndxOffset; // 39
	u16 OptionalSupport; // 41
	u16 ByteCount; // 43
} __attribute__((packed));

struct TreeDisconnectRequest_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u16 ByteCount; // 37
} __attribute__((packed));

struct TreeDisconnectResponse_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u16 ByteCount; // 37
} __attribute__((packed));

struct NetShareEnumRequest_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	struct SMBTransactionRequest_t smbTrans; // 37
	u16 ByteCount; // 65
	u8 ByteField[0]; // 67
} __attribute__((packed));

struct NetShareEnumResponse_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	struct SMBTransactionResponse_t smbTrans; // 37
	u16 ByteCount; // 57
	u8 ByteField[0]; // 59
} __attribute__((packed));

struct LogOffAndXRequest_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u8 smbAndxCmd; // 37
	u8 smbAndxReserved; // 38
	u16 smbAndxOffset; // 39
	u16 ByteCount; // 41
} __attribute__((packed));

struct LogOffAndXResponse_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u8 smbAndxCmd; // 37
	u8 smbAndxReserved; // 38
	u16 smbAndxOffset; // 39
	u16 ByteCount; // 41
} __attribute__((packed));

struct EchoRequest_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u16 EchoCount; // 37
	u16 ByteCount; // 39
	u8 ByteField[0]; // 41
} __attribute__((packed));

struct EchoResponse_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u16 SequenceNumber; // 37
	u16 ByteCount; // 39
	u8 ByteField[0]; // 41
} __attribute__((packed));

struct QueryInformationDiskRequest_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u16 ByteCount; // 37
	u8 ByteField[0]; // 39
} __attribute__((packed));

struct QueryInformationDiskResponse_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u16 TotalUnits; // 37
	u16 BlocksPerUnit; // 39
	u16 BlockSize; // 41
	u16 FreeUnits; // 43
	u16 Reserved; // 45
	u16 ByteCount; // 47
} __attribute__((packed));

struct QueryPathInformationRequestParam_t {
	u16 LevelOfInterest;
	u32 Reserved;
	u8 FileName[0];
} __attribute__((packed));

struct QueryPathInformationRequest_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	struct SMBTransactionRequest_t smbTrans; // 37
	u16 SubCommand; // 65
	u16 ByteCount; // 67
	u8 ByteField[0]; // 69
} __attribute__((packed));

struct QueryPathInformationResponse_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	struct SMBTransactionResponse_t smbTrans; // 37
	u16 ByteCount; // 57
	u8 ByteField[0]; // 59
} __attribute__((packed));

struct BasicFileInfo_t {
	s64 Created;
	s64 LastAccess;
	s64 LastWrite;
	s64 Change;
	u32 FileAttributes;
} __attribute__((packed));

struct StandardFileInfo_t {
	u64 AllocationSize;
	u64 EndOfFile;
	u32 LinkCount;
	u8 DeletePending;
	u8 IsDirectory;
} __attribute__((packed));

struct FindFirst2RequestParam_t {
	u16 SearchAttributes;
	u16 SearchCount;
	u16 Flags;
	u16 LevelOfInterest;
	u32 StorageType;
	u8 SearchPattern[0];
} __attribute__((packed));

struct FindNext2RequestParam_t {
	u16 SearchID;
	u16 SearchCount;
	u16 LevelOfInterest;
	u32 ResumeKey;
	u16 Flags;
	u8 SearchPattern[0];
} __attribute__((packed));

struct FindFirstNext2Request_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	struct SMBTransactionRequest_t smbTrans; // 37
	u16 SubCommand; // 65
	u16 ByteCount; // 67
	u8 ByteField[0]; // 69
} __attribute__((packed));

struct FindFirstNext2ResponseParam_t {
	u16 SearchID;
	u16 SearchCount;
	u16 EndOfSearch;
	u16 EAErrorOffset;
	u16 LastNameOffset;
} __attribute__((packed));

struct FindFirst2ResponseData_t {
	u32 NextEntryOffset;
	u32 FileIndex;
	s64 Created;
	s64 LastAccess;
	s64 LastWrite;
	s64 Change;
	u64 EndOfFile;
	u64 AllocationSize;
	u32 FileAttributes;
	u32 FileNameLen;
	u32 EAListLength;
	u16 ShortFileNameLen;
	u8 ShortFileName[24];
	u8 FileName[0];
} __attribute__((packed));

struct FindFirstNext2Response_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	struct SMBTransactionResponse_t smbTrans; // 37
	u16 ByteCount; // 57
	u8 ByteField[0]; // 59
} __attribute__((packed));

struct NTCreateAndXRequest_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u8 smbAndxCmd; // 37
	u8 smbAndxReserved; // 38
	u16 smbAndxOffset; // 39
	u8 reserved; // 41
	short NameLength; // 42
	u32 Flags; // 44
	u32 RootDirectoryFid; // 48
	u32 AccessMask; // 52
	u64 AllocationSize; // 56
	u32 FileAttributes; // 64
	u32 ShareAccess; // 68
	u32 CreateDisposition; // 72
	u32 CreateOptions; // 76
	u32 ImpersonationLevel; // 80
	u8 SecurityFlags; // 84
	u16 ByteCount; // 85
	char ByteField[0]; // 87
} __attribute__((packed));

struct NTCreateAndXResponse_t { // size = 107
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u8 smbAndxCmd; // 37
	u8 smbAndxReserved; // 38
	u16 smbAndxOffset; // 39
	u8 OplockLevel; // 41
	u16 FID; // 42
	u32 Action; // 44
	s64 Created; // 48
	s64 LastAccess; // 56
	s64 LastWrite; // 64
	s64 Changed; // 72
	u32 FileAttributes; // 80
	u64 AllocationSize; // 84
	u64 FileSize; // 92
	u16 FileType; // 100
	u16 IPCState; // 102
	u8 IsDirectory; // 104
	u16 ByteCount; // 105
} __attribute__((packed));

struct OpenAndXRequest_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u8 smbAndxCmd; // 37
	u8 smbAndxReserved; // 38
	u16 smbAndxOffset; // 39
	u16 Flags; // 41
	u16 AccessMask; // 43
	u16 SearchAttributes; // 45
	u16 FileAttributes; // 47
	u8 Created[4]; // 49
	u16 CreateOptions; // 53
	u32 AllocationSize; // 55
	u32 reserved[2]; // 59
	u16 ByteCount; // 67
	u8 ByteField[0]; // 69
} __attribute__((packed));

struct OpenAndXResponse_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u8 smbAndxCmd; // 37
	u8 smbAndxReserved; // 38
	u16 smbAndxOffset; // 39
	u16 FID; // 41
	u16 FileAttributes; // 43
	u8 LastWrite[4]; // 45
	u32 FileSize; // 49
	u16 GrantedAccess; // 53
	u16 FileType; // 55
	u16 IPCState; // 57
	u16 Action; // 59
	u32 ServerFID; // 61
	u16 reserved; // 65
	u16 ByteCount; // 67
} __attribute__((packed));

struct ReadAndXRequest_t { // size = 63
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u8 smbAndxCmd; // 37
	u8 smbAndxReserved; // 38
	u16 smbAndxOffset; // 39
	u16 FID; // 41
	u32 OffsetLow; // 43
	u16 MaxCountLow; // 47
	u16 MinCount; // 49
	u32 MaxCountHigh; // 51
	u16 Remaining; // 55
	u32 OffsetHigh; // 57
	u16 ByteCount; // 61
} __attribute__((packed));

struct ReadAndXResponse_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u8 smbAndxCmd; // 37
	u8 smbAndxReserved; // 38
	u16 smbAndxOffset; // 39
	u16 Remaining; // 41
	u16 DataCompactionMode; // 43
	u16 reserved; // 45
	u16 DataLengthLow; // 47
	u16 DataOffset; // 49
	u32 DataLengthHigh; // 51
	u8 reserved2[6]; // 55
	u16 ByteCount; // 61
} __attribute__((packed));

struct WriteAndXRequest_t { // size = 63
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u8 smbAndxCmd; // 37
	u8 smbAndxReserved; // 38
	u16 smbAndxOffset; // 39
	u16 FID; // 41
	u32 OffsetLow; // 43
	u32 Reserved; // 47
	u16 WriteMode; // 51
	u16 Remaining; // 53
	u16 DataLengthHigh; // 55
	u16 DataLengthLow; // 57
	u16 DataOffset; // 59
	u32 OffsetHigh; // 61
	u16 ByteCount; // 65
} __attribute__((packed));

struct WriteAndXResponse_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u8 smbAndxCmd; // 37
	u8 smbAndxReserved; // 38
	u16 smbAndxOffset; // 39
	u16 Count; // 41
	u16 Remaining; // 43
	u16 CountHigh; // 45
	u16 Reserved; // 47
} __attribute__((packed));

struct CloseRequest_t { // size = 45
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u16 FID; // 37
	u32 LastWrite; // 39
	u16 ByteCount; // 43
} __attribute__((packed));

struct CloseResponse_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u16 ByteCount; // 37
} __attribute__((packed));

struct DeleteRequest_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u16 SearchAttributes; // 37
	u16 ByteCount; // 39
	u8 BufferFormat; // 41
	u8 FileName[0]; // 42
} __attribute__((packed));

struct DeleteResponse_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u16 ByteCount; // 37
} __attribute__((packed));

struct ManageDirectoryRequest_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u16 ByteCount; // 37
	u8 BufferFormat; // 39
	u8 DirectoryName[0]; // 40
} __attribute__((packed));

struct ManageDirectoryResponse_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u16 ByteCount; // 37
} __attribute__((packed));

struct RenameRequest_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u16 SearchAttributes; // 37
	u16 ByteCount; // 39
	u8 ByteField[0]; // 41
} __attribute__((packed));

struct RenameResponse_t {
	struct SMBHeader_t smbH; // 0
	u8 smbWordcount; // 36
	u16 ByteCount; // 37
} __attribute__((packed));


static server_specs_t server_specs;

struct ReadAndXRequest_t smb_Read_Request = {
	{ //0x3b000000,
		0x0000003b,
		SMB_MAGIC,
		SMB_COM_READ_ANDX,
		0, 0, 0, 0, "\0", 0, 0, 0, 0
	},
	12,
	SMB_COM_NONE,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

struct WriteAndXRequest_t smb_Write_Request = {
	{ 0,
		SMB_MAGIC,
		SMB_COM_WRITE_ANDX,
		0, 0, 0, 0, "\0", 0, 0, 0, 0},
	14,
	SMB_COM_NONE,
	0, 0, 0, 0, 0, 0x01, 0, 0, 0, 0x3f, 0 // 0x01 is WriteThrough mode and 0x3f is DataOffset
};

#define LM_AUTH 	0
#define NTLM_AUTH 	1

static int main_socket = -1;

static u8 SMB_buf[MAX_SMB_BUF + 1024] __attribute__((aligned(64)));

//-------------------------------------------------------------------------

server_specs_t *getServerSpecs(void) {
	return &server_specs;
}

//-------------------------------------------------------------------------

static int rawTCP_SetSessionHeader(u32 size) // Write Session Service header: careful it's raw TCP transport here and not NBT transport
{
	// maximum for raw TCP transport (24 bits) !!!
	SMB_buf[0] = 0;
	SMB_buf[1] = (size >> 16) & 0xff;
	SMB_buf[2] = (size >> 8) & 0xff;
	SMB_buf[3] = size & 0xff;

	// smb_printf("rawTCP_SetSessionHeader : %d\n", size);
	return (int) size;
}

//-------------------------------------------------------------------------

static int rawTCP_GetSessionHeader(void) // Read Session Service header: careful it's raw TCP transport here and not NBT transport
{
	register u32 size;

	size = SMB_buf[3];
	size |= SMB_buf[2] << 8;
	size |= SMB_buf[1] << 16;

	// smb_printf("rawTCP_GetSessionHeader : %d\n", size);
	return (int) size;
}

//-------------------------------------------------------------------------

static int OpenTCPSession(struct in_addr dst_IP, u16 dst_port, int *sock) {
	register int sck, ret;
	struct sockaddr_in sock_addr;

	*sock = -1;

	// Creating socket
	sck = lwip_socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sck < 0)
		return -1;

	*sock = sck;

	memset(&sock_addr, 0, sizeof (sock_addr));
	sock_addr.sin_addr = dst_IP;
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(dst_port);

	ret = lwip_connect(sck, (struct sockaddr *) &sock_addr, sizeof (sock_addr));
	if (ret < 0)
		return -2;

	return 0;
}

//-------------------------------------------------------------------------

static int RecvTimeout(int sock, void *buf, int bsize, int timeout_ms) {
	register int ret;
	struct pollfd pollfd[1];

	pollfd->fd = sock;
	pollfd->events = POLLIN;
	pollfd->revents = 0;

	ret = poll(pollfd, 1, timeout_ms);

	// a result less than 0 is an error
	if (ret < 0)
		return -1;

	// 0 is a timeout
	if (ret == 0) {
		return 0;
	}

	// receive the packet
	ret = lwip_recv(sock, buf, bsize, 0);
	if (ret < 0)
		return -2;

	return ret;
}

//-------------------------------------------------------------------------

static int GetSMBServerReply(void) {
	register int rcv_size, totalpkt_size, pkt_size;

	rcv_size = lwip_send(main_socket, SMB_buf, rawTCP_GetSessionHeader() + 4, 0);

	if (rcv_size <= 0) {
		smb_printf("lwip_send error = %d\n", rcv_size);
		return -1;
	}

receive:
	rcv_size = RecvTimeout(main_socket, SMB_buf, sizeof (SMB_buf), 10000); // 10s before the packet is considered lost
	if (rcv_size <= 0) {
		smb_printf("RecvTimeout: timeout\n");
		return -2;
	}

	if (SMB_buf[0] != 0) // dropping NBSS Session Keep alive
		goto receive;

	// Handle fragmented packets
	totalpkt_size = rawTCP_GetSessionHeader() + 4;

	while (rcv_size < totalpkt_size) {
		pkt_size = RecvTimeout(main_socket, &SMB_buf[rcv_size], sizeof (SMB_buf) - rcv_size, 3000); // 3s before the packet is considered lost
		if (pkt_size <= 0) {
			smb_printf("pkt_size <= 0 / RecvTimeout: timeout\n");
			return -2;
		}
		rcv_size += pkt_size;
	}

	return rcv_size;
}

//-------------------------------------------------------------------------

int smb_NegociateProtocol(void) {
	static char *dialect = "NT LM 0.12";
	register int r, length, retry_count;
	struct NegociateProtocolRequest_t *NPR = (struct NegociateProtocolRequest_t *) SMB_buf;

	retry_count = 0;

negociate_retry:

	memset(SMB_buf, 0, sizeof (SMB_buf));

	NPR->smbH.Magic = SMB_MAGIC;
	NPR->smbH.Cmd = SMB_COM_NEGOCIATE;
	NPR->smbH.Flags = SMB_FLAGS_CASELESS_PATHNAMES;
	NPR->smbH.Flags2 = bswap_16(SMB_FLAGS2_KNOWS_LONG_NAMES | SMB_FLAGS2_32BIT_STATUS);
	length = strlen(dialect);
	NPR->ByteCount = bswap_16(length + 2);
	NPR->DialectFormat = 0x02;
	strcpy(NPR->DialectName, dialect);

	rawTCP_SetSessionHeader(37 + length);
	r = GetSMBServerReply();

	if (r <= 0) {
		smb_printf("bad response from server\n");
		goto negociate_error;
	}

	struct NegociateProtocolResponse_t *NPRsp = (struct NegociateProtocolResponse_t *) SMB_buf;

	// check sanity of SMB header
	if (NPRsp->smbH.Magic != SMB_MAGIC) {
		smb_printf("Wrong header in response.\n");
		goto negociate_error;
	}

	// check there's no error
	if (bswap_16(NPRsp->smbH.Eclass) != STATUS_SUCCESS) {
		smb_printf("Eclass %08x\n", bswap_16(NPRsp->smbH.Eclass));
		goto negociate_error;
	}

	if (NPRsp->smbWordcount != 17) {
		smb_printf("smbWordcount %08x\n", bswap_16(NPRsp->smbH.Eclass));
		goto negociate_error;
	}

	if (bswap_32(NPRsp->Capabilities) & SERVER_CAP_UNICODE)
		server_specs.StringsCF = 2;
	else
		server_specs.StringsCF = 1;

	if (bswap_32(NPRsp->Capabilities) & SERVER_CAP_NT_SMBS)
		server_specs.SupportsNTSMB = 1;
	else
		server_specs.SupportsNTSMB = 0;

	if (NPRsp->SecurityMode & NEGOCIATE_SECURITY_USER_LEVEL)
		server_specs.SecurityMode = SERVER_USER_SECURITY_LEVEL;
	else
		server_specs.SecurityMode = SERVER_SHARE_SECURITY_LEVEL;

	if (NPRsp->SecurityMode & NEGOCIATE_SECURITY_CHALLENGE_RESPONSE)
		server_specs.PasswordType = SERVER_USE_ENCRYPTED_PASSWORD;
	else
		server_specs.PasswordType = SERVER_USE_PLAINTEXT_PASSWORD;

	// copy to global struct to keep needed information for further communication
	server_specs.MaxBufferSize = bswap_32(NPRsp->MaxBufferSize);
	server_specs.MaxMpxCount = bswap_16(NPRsp->MaxMpxCount);
	server_specs.SessionKey = bswap_32(NPRsp->SessionKey);
	memcpy(&server_specs.EncryptionKey[0], &NPRsp->ByteField[0], NPRsp->KeyLength);
	memcpy(&server_specs.PrimaryDomainServerName[0], &NPRsp->ByteField[NPRsp->KeyLength], 64);

	return 0;

negociate_error:
	retry_count++;

	if (retry_count < 3)
		goto negociate_retry;

	return -1;
}

//-------------------------------------------------------------------------

static int AddPassword(char *Password, int PasswordType, int AuthType, u16 *AnsiPassLen, u16 *UnicodePassLen, u8 *Buffer) {
	u8 passwordhash[16];
	u8 LMresponse[24];
	u16 passwordlen = 0;

	if ((Password) && (PasswordType != NO_PASSWORD)) {
		if (server_specs.PasswordType == SERVER_USE_ENCRYPTED_PASSWORD) {
			passwordlen = 24;
			switch (PasswordType) {
				case HASHED_PASSWORD:
					if (AuthType == LM_AUTH) {
						memcpy(passwordhash, &Password[0], 16);
                                                *AnsiPassLen = passwordlen;
                        
					}
					if (AuthType == NTLM_AUTH) {
						memcpy(passwordhash, &Password[16], 16);
                                            *UnicodePassLen = passwordlen;
					}
					break;

				default:
					if (AuthType == LM_AUTH) {
						LM_Password_Hash(Password, passwordhash);
                                                 *AnsiPassLen = passwordlen;
					} else if (AuthType == NTLM_AUTH) {
						NTLM_Password_Hash(Password, passwordhash);
                                                 *UnicodePassLen = passwordlen;
					}
			}
			LM_Response(passwordhash, server_specs.EncryptionKey, LMresponse);
			memcpy(Buffer, LMresponse, passwordlen);
		} else if (server_specs.PasswordType == SERVER_USE_PLAINTEXT_PASSWORD) {
			// It seems that PlainText passwords and Unicode isn't meant to be...
			passwordlen = strlen(Password);
			if (passwordlen > 14)
				passwordlen = 14;
			else if (passwordlen == 0)
				passwordlen = 1;
                         *AnsiPassLen = passwordlen;
			memcpy(Buffer, Password, passwordlen);
		}
	} else {
		if (server_specs.SecurityMode == SERVER_SHARE_SECURITY_LEVEL) {
			passwordlen = 1;
                         *AnsiPassLen = passwordlen;
			Buffer[0] = 0;
		}
	}

	return passwordlen;
}

//-------------------------------------------------------------------------

int smb_SessionSetupAndX(char *User, char *Password, int PasswordType) {
	struct SessionSetupAndXRequest_t *SSR = (struct SessionSetupAndXRequest_t *) SMB_buf;
	register int r, i, offset, CF;
	int passwordlen = 0;
	int AuthType = NTLM_AUTH;

lbl_session_setup:

	memset(SMB_buf, 0, sizeof (SMB_buf));

	CF = server_specs.StringsCF;

	SSR->smbH.Magic = SMB_MAGIC;
	SSR->smbH.Cmd = SMB_COM_SESSION_SETUP_ANDX;
	SSR->smbH.Flags = SMB_FLAGS_CASELESS_PATHNAMES;
	SSR->smbH.Flags2 = SMB_FLAGS2_KNOWS_LONG_NAMES | SMB_FLAGS2_32BIT_STATUS;
	if (CF == 2)
		SSR->smbH.Flags2 |= SMB_FLAGS2_UNICODE_STRING;

	SSR->smbH.Flags2 = bswap_16(SSR->smbH.Flags2);

	SSR->smbWordcount = 13;
	SSR->smbAndxCmd = SMB_COM_NONE; // no ANDX command
	SSR->MaxBufferSize = server_specs.MaxBufferSize > 65535 ? 65535 : (u16) server_specs.MaxBufferSize;

	SSR->MaxBufferSize = bswap_16(SSR->MaxBufferSize);

	SSR->MaxMpxCount = server_specs.MaxMpxCount >= 2 ? 2 : (u16) server_specs.MaxMpxCount;

	SSR->MaxMpxCount = bswap_16(SSR->MaxMpxCount);

	SSR->VCNumber = bswap_16(1);
	SSR->SessionKey = bswap_32(server_specs.SessionKey);
	SSR->Capabilities = bswap_32(CLIENT_CAP_LARGE_READX | CLIENT_CAP_UNICODE | CLIENT_CAP_LARGE_FILES | CLIENT_CAP_NT_SMBS | CLIENT_CAP_STATUS32);

	// Fill ByteField
	offset = 0;

	if (server_specs.SecurityMode == SERVER_USER_SECURITY_LEVEL) {
		passwordlen = AddPassword(Password, PasswordType, AuthType, &SSR->AnsiPasswordLength, &SSR->UnicodePasswordLength, &SSR->ByteField[0]);
		offset += passwordlen;

		SSR->AnsiPasswordLength = bswap_16(SSR->AnsiPasswordLength);
		SSR->UnicodePasswordLength = bswap_16(SSR->UnicodePasswordLength);
	}

	if ((CF == 2) && (!(passwordlen & 1)))
		offset += 1; // pad needed only for unicode as aligment fix if password length is even

	for (i = 0; i < strlen(User); i++) {
		SSR->ByteField[offset] = User[i]; // add User name
		offset += CF;
	}
	offset += CF; // null terminator

	for (i = 0; server_specs.PrimaryDomainServerName[i] != 0; i += CF) {
		SSR->ByteField[offset] = server_specs.PrimaryDomainServerName[i]; // PrimaryDomain, acquired from Negociate Protocol Response Datas
		offset += CF;
	}
	offset += CF; // null terminator

	for (i = 0; i < (CF << 1); i++)
		SSR->ByteField[offset++] = 0; // NativeOS, NativeLanMan	

	SSR->ByteCount = bswap_16(offset);

	rawTCP_SetSessionHeader(61 + offset);
	r = GetSMBServerReply();
	if (r <= 0)
		return -3;

	struct SessionSetupAndXResponse_t *SSRsp = (struct SessionSetupAndXResponse_t *) SMB_buf;

	// check sanity of SMB header
	if (SSRsp->smbH.Magic != SMB_MAGIC) {
		smb_printf("Wrong Header in response.\n");
		return -1;
	}

	// :s
	SSRsp->smbH.Eclass = bswap_16(SSRsp->smbH.Eclass);
	SSRsp->smbH.Ecode = bswap_16(SSRsp->smbH.Ecode);

	// check there's no auth failure
	if ((server_specs.SecurityMode == SERVER_USER_SECURITY_LEVEL)
			&& ((SSRsp->smbH.Eclass | (SSRsp->smbH.Ecode << 16)) == STATUS_LOGON_FAILURE)
			&& (AuthType == NTLM_AUTH)) {
		AuthType = LM_AUTH;
		goto lbl_session_setup;
	}

	// check there's no error (NT STATUS error type!)
	if ((SSRsp->smbH.Eclass | SSRsp->smbH.Ecode) != STATUS_SUCCESS) {
		smb_printf("Error: %04x%04x\n", SSRsp->smbH.Ecode & 0xFFFF, SSRsp->smbH.Eclass & 0xFFFF);
		return -2;
	}

	// return UID
	return (int) bswap_16(SSRsp->smbH.UID);
}

//-------------------------------------------------------------------------

int smb_TreeConnectAndX(int UID, char *ShareName, char *Password, int PasswordType) // PasswordType: 0 = PlainText, 1 = Hash
{
	struct TreeConnectAndXRequest_t *TCR = (struct TreeConnectAndXRequest_t *) SMB_buf;
	register int r, i, offset, CF;
	int passwordlen = 0;
	int AuthType = NTLM_AUTH;

lbl_tree_connect:

	memset(SMB_buf, 0, sizeof (SMB_buf));

	CF = server_specs.StringsCF;

	TCR->smbH.Magic = SMB_MAGIC;
	TCR->smbH.Cmd = SMB_COM_TREE_CONNECT_ANDX;
	TCR->smbH.Flags = SMB_FLAGS_CASELESS_PATHNAMES;
	TCR->smbH.Flags2 = SMB_FLAGS2_KNOWS_LONG_NAMES | SMB_FLAGS2_32BIT_STATUS;
	if (CF == 2)
		TCR->smbH.Flags2 |= SMB_FLAGS2_UNICODE_STRING;

	TCR->smbH.Flags2 = bswap_16(TCR->smbH.Flags2);

	TCR->smbH.UID = bswap_16(UID);

	TCR->smbWordcount = 4;
	TCR->smbAndxCmd = SMB_COM_NONE; // no ANDX command

	// Fill ByteField
	offset = 0;

	if (server_specs.SecurityMode == SERVER_SHARE_SECURITY_LEVEL)
		passwordlen = AddPassword(Password, PasswordType, AuthType, &TCR->PasswordLength, &TCR->PasswordLength, &TCR->ByteField[offset]);
	else {
		passwordlen = 1;
		TCR->PasswordLength = passwordlen;
	}
	// bswap
	TCR->PasswordLength = bswap_16(TCR->PasswordLength);

	offset += passwordlen;

	if ((CF == 2) && (!(passwordlen & 1)))
		offset += 1; // pad needed only for unicode as aligment fix is password len is even

	for (i = 0; i < strlen(ShareName); i++) {
		TCR->ByteField[offset] = ShareName[i]; // add Share name
		offset += CF;
	}
	offset += CF; // null terminator

	memcpy(&TCR->ByteField[offset], "?????\0", 6); // Service, any type of device
	offset += 6;

	TCR->ByteCount = bswap_16(offset);

	rawTCP_SetSessionHeader(43 + offset);
	r = GetSMBServerReply();
	if (r <= 0)
		return -3;

	struct TreeConnectAndXResponse_t *TCRsp = (struct TreeConnectAndXResponse_t *) SMB_buf;

	// check sanity of SMB header
	if (TCRsp->smbH.Magic != SMB_MAGIC) {
		smb_printf("Wrong magic in response.\n");
		return -1;
	}


	TCRsp->smbH.Eclass = bswap_16(TCRsp->smbH.Eclass);
	TCRsp->smbH.Ecode = bswap_16(TCRsp->smbH.Ecode);

	// check there's no auth failure
	if ((server_specs.SecurityMode == SERVER_USER_SECURITY_LEVEL)
			&& ((TCRsp->smbH.Eclass | (TCRsp->smbH.Ecode << 16)) == STATUS_LOGON_FAILURE)
			&& (AuthType == NTLM_AUTH)) {
		AuthType = LM_AUTH;
		goto lbl_tree_connect;
	}

	// check there's no error (NT STATUS error type!)
	if ((TCRsp->smbH.Eclass | TCRsp->smbH.Ecode) != STATUS_SUCCESS) {
		smb_printf("Error: %04x%04x\n", TCRsp->smbH.Ecode & 0xFFFF, TCRsp->smbH.Eclass & 0xFFFF);
		return -2;
	}

	// return TID
	return (int) bswap_16(TCRsp->smbH.TID);
}

//-------------------------------------------------------------------------

int smb_NetShareEnum(int UID, int TID, ShareEntry_t *shareEntries, int index, int maxEntries) {
	register int r, i;
	register int count = 0;
	struct NetShareEnumRequest_t *NSER = (struct NetShareEnumRequest_t *) SMB_buf;

	memset(SMB_buf, 0, sizeof (SMB_buf));

	NSER->smbH.Magic = SMB_MAGIC;
	NSER->smbH.Cmd = SMB_COM_TRANSACTION;
	NSER->smbH.UID = bswap_16((u16) UID);
	NSER->smbH.TID = bswap_16((u16) TID);
	NSER->smbWordcount = 14;

	NSER->smbTrans.TotalParamCount = NSER->smbTrans.ParamCount = bswap_16(19);

	NSER->smbTrans.MaxParamCount = bswap_16(1024);
	NSER->smbTrans.MaxDataCount = bswap_16(8096);
	NSER->smbTrans.ParamOffset = bswap_16(76);
	NSER->smbTrans.DataOffset = bswap_16(95);

	NSER->ByteCount = bswap_16(32);

	// SMB PIPE PROTOCOL
	// Transaction Name: "\PIPE\LANMAN"
	// Function Code : 0x0000 = NetShareEnum
	// Parameter Descriptor: "WrLeh"
	// Return Descriptor: "B13BWz"
	// Detail Level: 0x0001
	// Receive Buffer Length: 0x1fa0
	memcpy(&NSER->ByteField[0], "\\PIPE\\LANMAN\0\0\0WrLeh\0B13BWz\0\x01\0\xa0\x1f", 32);

	rawTCP_SetSessionHeader(95);
	r = GetSMBServerReply();
	if (r <= 0) {
		smb_printf("Response error.\n");
		return -3;
	}

	struct NetShareEnumResponse_t *NSERsp = (struct NetShareEnumResponse_t *) SMB_buf;

	// check sanity of SMB header
	if (NSERsp->smbH.Magic != SMB_MAGIC) {
		smb_printf("Wrong magic in response.\n");
		return -1;
	}

	NSERsp->smbH.Eclass = bswap_16(NSERsp->smbH.Eclass);
	NSERsp->smbH.Ecode = bswap_16(NSERsp->smbH.Ecode);

	// check there's no error
	if ((NSERsp->smbH.Eclass | NSERsp->smbH.Ecode) != STATUS_SUCCESS) {
		smb_printf("Error: %04x%04x\n", NSERsp->smbH.Ecode & 0xFFFF, NSERsp->smbH.Eclass & 0xFFFF);
		return -2;
	}

	// API status must be 0
	if (*((u16 *) & SMB_buf[bswap_16(NSERsp->smbTrans.ParamOffset) + 4]) != 0)
		return -1;

	// available entries
	int AvailableEntries = (int) (bswap_16(*(u16 *) & SMB_buf[bswap_16(NSERsp->smbTrans.ParamOffset) + 4 + 6]));

	// data start
	u8 *data = (u8 *) & SMB_buf[bswap_16(NSERsp->smbTrans.DataOffset) + 4];
	u8 *p = (u8 *) data;

	for (i = 0; i < AvailableEntries; i++) {

		// calculate the padding after the Share name 
		int padding = (strlen(p) + 1 + 2) % 16 ? 16 - ((strlen(p) + 1) % 16) : 0;
		u16 dir_tree_type = *((u16 *) & p[strlen(p) + 1 + padding - 2]);
		dir_tree_type = bswap_16(dir_tree_type);

		if (dir_tree_type == 0) { // Directory Tree type
			if (maxEntries > 0) {
				if ((count < maxEntries) && (i >= index)) {
					count++;
					strncpy(shareEntries->ShareName, p, 256);
					strncpy(shareEntries->ShareComment, &data[*((u16 *) & p[strlen(p) + 1 + padding])], 256);
					shareEntries++;
				}
			} else // if maxEntries is 0 then we're just counting shares
				count++;
		}
		p += strlen(p) + 1 + padding + 4;
	}

	return count;
}

//-------------------------------------------------------------------------

int smb_QueryInformationDisk(int UID, int TID, smbQueryDiskInfo_out_t *QueryInformationDisk) {
	register int r;
	struct QueryInformationDiskRequest_t *QIDR = (struct QueryInformationDiskRequest_t *) SMB_buf;

	memset(SMB_buf, 0, sizeof (SMB_buf));

	QIDR->smbH.Magic = SMB_MAGIC;
	QIDR->smbH.Cmd = SMB_COM_QUERY_INFORMATION_DISK;
	QIDR->smbH.UID = bswap_16((u16) UID);
	QIDR->smbH.TID = bswap_16((u16) TID);

	rawTCP_SetSessionHeader(35);
	r = GetSMBServerReply();
	if (r <= 0) {
		smb_printf("Response error.\n");
		return -3;
	}

	struct QueryInformationDiskResponse_t *QIDRsp = (struct QueryInformationDiskResponse_t *) SMB_buf;

	// check sanity of SMB header
	if (QIDRsp->smbH.Magic != SMB_MAGIC) {
		smb_printf("Wrong magic in response.\n");
		return -1;
	}

	QIDRsp->smbH.Eclass = bswap_16(QIDRsp->smbH.Eclass);
	QIDRsp->smbH.Ecode = bswap_16(QIDRsp->smbH.Ecode);

	// check there's no error
	if ((QIDRsp->smbH.Eclass | QIDRsp->smbH.Ecode) != STATUS_SUCCESS) {
		smb_printf("Error: %04x%04x\n", QIDRsp->smbH.Ecode & 0xFFFF, QIDRsp->smbH.Eclass & 0xFFFF);
		return -2;
	}

	QueryInformationDisk->TotalUnits = bswap_16(QIDRsp->TotalUnits);
	QueryInformationDisk->BlocksPerUnit = bswap_16(QIDRsp->BlocksPerUnit);
	QueryInformationDisk->BlockSize = bswap_16(QIDRsp->BlockSize);
	QueryInformationDisk->FreeUnits = bswap_16(QIDRsp->FreeUnits);

	return 0;
}

//-------------------------------------------------------------------------

int smb_QueryPathInformation(int UID, int TID, PathInformation_t *Info, char *Path) {
	register int r, PathLen, CF, i, queryType;
	struct QueryPathInformationRequest_t *QPIR = (struct QueryPathInformationRequest_t *) SMB_buf;

	queryType = SMB_QUERY_FILE_BASIC_INFO;

query:

	memset(SMB_buf, 0, sizeof (SMB_buf));

	CF = server_specs.StringsCF;

	QPIR->smbH.Magic = SMB_MAGIC;
	QPIR->smbH.Cmd = SMB_COM_TRANSACTION2;
	QPIR->smbH.Flags = SMB_FLAGS_CANONICAL_PATHNAMES; //| SMB_FLAGS_CASELESS_PATHNAMES;
	QPIR->smbH.Flags2 = SMB_FLAGS2_KNOWS_LONG_NAMES;
	if (CF == 2)
		QPIR->smbH.Flags2 |= SMB_FLAGS2_UNICODE_STRING;

	QPIR->smbH.Flags2 = bswap_16(QPIR->smbH.Flags2);
	QPIR->smbH.UID = bswap_16((u16) UID);
	QPIR->smbH.TID = bswap_16((u16) TID);
	QPIR->smbWordcount = 15;

	QPIR->smbTrans.SetupCount = 1;
	QPIR->SubCommand = bswap_16(TRANS2_QUERY_PATH_INFORMATION);

	QPIR->smbTrans.ParamOffset = 68;
	QPIR->smbTrans.MaxParamCount = 256; // Max Parameters len in reply
	QPIR->smbTrans.MaxDataCount = 16384; // Max Data len in reply

	struct QueryPathInformationRequestParam_t *QPIRParam = (struct QueryPathInformationRequestParam_t *) &SMB_buf[QPIR->smbTrans.ParamOffset + 4];

	QPIRParam->LevelOfInterest = bswap_16(queryType);

	PathLen = 0;
	for (i = 0; i < strlen(Path); i++) {
		QPIRParam->FileName[PathLen] = Path[i]; // add Path
		PathLen += CF;
	}
	PathLen += CF; // null terminator

	QPIR->smbTrans.TotalParamCount = QPIR->smbTrans.ParamCount = 2 + 4 + PathLen;

	QPIR->ByteCount = 3 + QPIR->smbTrans.TotalParamCount;

	QPIR->smbTrans.DataOffset = QPIR->smbTrans.ParamOffset + QPIR->smbTrans.TotalParamCount;


	rawTCP_SetSessionHeader(QPIR->smbTrans.DataOffset);


	QPIR->smbTrans.ParamOffset = bswap_16(QPIR->smbTrans.ParamOffset);
	QPIR->smbTrans.MaxParamCount = bswap_16(QPIR->smbTrans.MaxParamCount);
	QPIR->smbTrans.MaxDataCount = bswap_16(QPIR->smbTrans.MaxDataCount);
	QPIR->smbTrans.TotalParamCount = bswap_16(QPIR->smbTrans.TotalParamCount);
	QPIR->smbTrans.ParamCount = bswap_16(QPIR->smbTrans.ParamCount);
	QPIR->ByteCount = bswap_16(QPIR->ByteCount);
	QPIR->smbTrans.DataOffset = bswap_16(QPIR->smbTrans.DataOffset);

	r = GetSMBServerReply();
	if (r <= 0) {
		smb_printf("Response error.\n");
		return -3;
	}

	struct QueryPathInformationResponse_t *QPIRsp = (struct QueryPathInformationResponse_t *) SMB_buf;

	// check sanity of SMB header
	if (QPIRsp->smbH.Magic != SMB_MAGIC) {
		smb_printf("Wrong magic in response.\n");
		return -1;
	}

	QPIRsp->smbH.Eclass = bswap_16(QPIRsp->smbH.Eclass);
	QPIRsp->smbH.Ecode = bswap_16(QPIRsp->smbH.Ecode);

	// check there's no error
	if ((QPIRsp->smbH.Eclass | QPIRsp->smbH.Ecode) != STATUS_SUCCESS) {
		smb_printf("Error: %04x%04x\n", QPIRsp->smbH.Ecode & 0xFFFF, QPIRsp->smbH.Eclass & 0xFFFF);
		return -2;
	}

	if (queryType == SMB_QUERY_FILE_BASIC_INFO) {

		struct BasicFileInfo_t *BFI = (struct BasicFileInfo_t *) &SMB_buf[bswap_16(QPIRsp->smbTrans.DataOffset) + 4];

		Info->Created = bswap_64(BFI->Created);
		Info->LastAccess = bswap_64(BFI->LastAccess);
		Info->LastWrite = bswap_64(BFI->LastWrite);
		Info->Change = bswap_64(BFI->Change);
		Info->FileAttributes = bswap_32(BFI->FileAttributes);

		// a 2nd query is done with SMB_QUERY_FILE_STANDARD_INFO LevelOfInterest to get a valid 64bit size
		queryType = SMB_QUERY_FILE_STANDARD_INFO;
		goto query;
	} else if (queryType == SMB_QUERY_FILE_STANDARD_INFO) {

		struct StandardFileInfo_t *SFI = (struct StandardFileInfo_t *) &SMB_buf[bswap_16(QPIRsp->smbTrans.DataOffset) + 4];

		Info->AllocationSize = bswap_64(SFI->AllocationSize);
		Info->EndOfFile = bswap_64(SFI->EndOfFile);
		Info->LinkCount = bswap_64(SFI->LinkCount);
		Info->DeletePending = bswap_64(SFI->DeletePending);
		Info->IsDirectory = bswap_64(SFI->IsDirectory);
	}

	return 0;
}

//-------------------------------------------------------------------------

int smb_NTCreateAndX(int UID, int TID, char *filename, s64 *filesize, int mode) {
	struct NTCreateAndXRequest_t *NTCR = (struct NTCreateAndXRequest_t *) SMB_buf;
	register int r, i, offset, length, CF;

	memset(SMB_buf, 0, sizeof (SMB_buf));

	CF = server_specs.StringsCF;

	NTCR->smbH.Magic = SMB_MAGIC;
	NTCR->smbH.Cmd = SMB_COM_NT_CREATE_ANDX;
	NTCR->smbH.Flags = SMB_FLAGS_CANONICAL_PATHNAMES; //| SMB_FLAGS_CASELESS_PATHNAMES;
	NTCR->smbH.Flags2 = SMB_FLAGS2_KNOWS_LONG_NAMES;
	if (CF == 2)
		NTCR->smbH.Flags2 |= SMB_FLAGS2_UNICODE_STRING;
	NTCR->smbH.Flags2 = bswap_16(NTCR->smbH.Flags2);
	NTCR->smbH.UID = bswap_16((u16) UID);
	NTCR->smbH.TID = bswap_16((u16) TID);
	NTCR->smbWordcount = 24;
	NTCR->smbAndxCmd = SMB_COM_NONE; // no ANDX command
	NTCR->AccessMask = ((mode & O_RDWR) == O_RDWR || (mode & O_WRONLY)) ? 0x2019f : 0x20089;
	NTCR->AccessMask = bswap_32(NTCR->AccessMask);
	NTCR->FileAttributes = ((mode & O_RDWR) == O_RDWR || (mode & O_WRONLY)) ? EXT_ATTR_NORMAL : EXT_ATTR_READONLY;
	NTCR->FileAttributes = bswap_32(NTCR->FileAttributes);
	NTCR->ShareAccess = bswap_32(0x01); // Share in read mode only
	if (mode & O_CREAT)
		NTCR->CreateDisposition |= 0x02;
	if (mode & O_TRUNC)
		NTCR->CreateDisposition |= 0x04;
	else
		NTCR->CreateDisposition |= 0x01;
	if (NTCR->CreateDisposition == 0x06)
		NTCR->CreateDisposition = 0x05;

	NTCR->CreateDisposition = bswap_32(NTCR->CreateDisposition);
	NTCR->ImpersonationLevel = bswap_32(2);
	NTCR->SecurityFlags = 0x03;

	offset = 0;
	if (CF == 2)
		offset++; // pad needed only for unicode as aligment fix

	length = strlen(filename);
	for (i = 0; i < length; i++) {
		NTCR->ByteField[offset] = filename[i]; // add filename
		offset += CF;
	}
	offset += CF;

	NTCR->NameLength = bswap_16(length);
	if (CF == 2)
		NTCR->NameLength = NTCR->NameLength << 1;
	NTCR->ByteCount = bswap_16(offset);

	rawTCP_SetSessionHeader(84 + offset);
	r = GetSMBServerReply();
	if (r <= 0)
		return -3;

	struct NTCreateAndXResponse_t *NTCRsp = (struct NTCreateAndXResponse_t *) SMB_buf;

	// check sanity of SMB header
	if (NTCRsp->smbH.Magic != SMB_MAGIC) {
		smb_printf("Wrong magic in response.\n");
		return -1;
	}

	NTCRsp->smbH.Eclass = bswap_16(NTCRsp->smbH.Eclass);
	NTCRsp->smbH.Ecode = bswap_16(NTCRsp->smbH.Ecode);

	// check if access denied
	if ((NTCRsp->smbH.Eclass | (NTCRsp->smbH.Ecode << 16)) == STATUS_ACCESS_DENIED) {
		printf("STATUS_ACCESS_DENIED\n");
		return -2;
	}

	// check there's no error
	if ((NTCRsp->smbH.Eclass | NTCRsp->smbH.Ecode) != STATUS_SUCCESS) {
		smb_printf("Eclass: %08x\n Ecode: %08x\n", NTCRsp->smbH.Eclass, NTCRsp->smbH.Ecode);
		return (NTCRsp->smbH.Eclass | (NTCRsp->smbH.Ecode << 16));
	}

	*filesize = bswap_64(NTCRsp->FileSize);

	return (int) bswap_16(NTCRsp->FID);
}

//-------------------------------------------------------------------------

int smb_OpenAndX(int UID, int TID, char *filename, s64 *filesize, int mode) {
	// does not supports filesize > 4Gb, so we'll have to use
	// smb_QueryPathInformation to find the real size.
	// OpenAndX is needed for a few NAS units that doesn't supports
	// NT SMB commands set.

	struct OpenAndXRequest_t *OR = (struct OpenAndXRequest_t *) SMB_buf;
	register int r, i, offset, CF;

	if (server_specs.SupportsNTSMB)
		return smb_NTCreateAndX(UID, TID, filename, filesize, mode);

	memset(SMB_buf, 0, sizeof (SMB_buf));

	CF = server_specs.StringsCF;

	OR->smbH.Magic = SMB_MAGIC;
	OR->smbH.Cmd = SMB_COM_OPEN_ANDX;
	OR->smbH.Flags = SMB_FLAGS_CANONICAL_PATHNAMES; //| SMB_FLAGS_CASELESS_PATHNAMES;
	OR->smbH.Flags2 = SMB_FLAGS2_KNOWS_LONG_NAMES;
	if (CF == 2)
		OR->smbH.Flags2 |= SMB_FLAGS2_UNICODE_STRING;
	OR->smbH.Flags2 = bswap_16(OR->smbH.Flags2);
	OR->smbH.UID = bswap_16((u16) UID);
	OR->smbH.TID = bswap_16((u16) TID);
	OR->smbWordcount = 15;
	OR->smbAndxCmd = SMB_COM_NONE; // no ANDX command
	OR->AccessMask = ((mode & O_RDWR) == O_RDWR || (mode & O_WRONLY)) ? 0x02 : 0x00;
	OR->AccessMask = bswap_16(OR->AccessMask);
	OR->FileAttributes = ((mode & O_RDWR) == O_RDWR || (mode & O_WRONLY)) ? EXT_ATTR_NORMAL : EXT_ATTR_READONLY;
	OR->FileAttributes = bswap_16(OR->FileAttributes);
	if (mode & O_CREAT)
		OR->CreateOptions |= 0x10;
	if (mode & O_TRUNC)
		OR->CreateOptions |= 0x02;
	else
		OR->CreateOptions |= 0x01;
	OR->CreateOptions = bswap_16(OR->CreateOptions);

	offset = 0;
	if (CF == 2)
		offset++; // pad needed only for unicode as aligment fix

	for (i = 0; i < strlen(filename); i++) {
		OR->ByteField[offset] = filename[i]; // add filename
		offset += CF;
	}
	offset += CF;

	OR->ByteCount = bswap_16(offset);

	rawTCP_SetSessionHeader(66 + offset);
	r = GetSMBServerReply();
	if (r <= 0)
		return -3;

	struct OpenAndXResponse_t *ORsp = (struct OpenAndXResponse_t *) SMB_buf;

	// check sanity of SMB header
	if (ORsp->smbH.Magic != SMB_MAGIC) {
		smb_printf("Wrong magic in response.\n");
		return -1;
	}


	ORsp->smbH.Eclass = bswap_16(ORsp->smbH.Eclass);
	ORsp->smbH.Ecode = bswap_16(ORsp->smbH.Ecode);

	// check if access denied
	if ((ORsp->smbH.Eclass | (ORsp->smbH.Ecode << 16)) == STATUS_ACCESS_DENIED) {
		smb_printf("STATUS_ACCESS_DENIED.\n");
		return -2;
	}

	// check if access denied
	if ((ORsp->smbH.Eclass | (ORsp->smbH.Ecode << 16)) == STATUS_OBJECT_NAME_NOT_FOUND) {
		smb_printf("STATUS_OBJECT_NAME_NOT_FOUND.\n");
		return -3;
	}

	// check there's no error
	if ((ORsp->smbH.Eclass | ORsp->smbH.Ecode) != STATUS_SUCCESS) {
		smb_printf("Eclass: %08x\n Ecode: %08x\n", ORsp->smbH.Eclass, ORsp->smbH.Ecode);
		return (ORsp->smbH.Eclass | (ORsp->smbH.Ecode << 16));
	}

	*filesize = bswap_32(ORsp->FileSize);

	return (int) bswap_16(ORsp->FID);
}

//-------------------------------------------------------------------------

int smb_ReadAndX(int UID, int TID, int FID, s64 fileoffset, void *readbuf, u16 nbytes) {
	struct ReadAndXRequest_t *RR = (struct ReadAndXRequest_t *) SMB_buf;
	register int r;

	memcpy(RR, &smb_Read_Request.smbH.sessionHeader, sizeof (struct ReadAndXRequest_t));

	RR->smbH.UID = bswap_16((u16) UID);
	RR->smbH.TID = bswap_16((u16) TID);
	RR->FID = bswap_16((u16) FID);
	RR->OffsetLow = bswap_32((u32) (fileoffset & 0xffffffff));
	RR->OffsetHigh = bswap_32((u32) ((fileoffset >> 32) & 0xffffffff));
	RR->MaxCountLow = bswap_16(nbytes);

	r = GetSMBServerReply();
	if (r <= 0)
		return -3;

	struct ReadAndXResponse_t *RRsp = (struct ReadAndXResponse_t *) SMB_buf;

	// check sanity of SMB header
	if (RRsp->smbH.Magic != SMB_MAGIC) {
		smb_printf("Wrong magic in response.\n");
		return -1;
	}

	RRsp->smbH.Eclass = bswap_16(RRsp->smbH.Eclass);
	RRsp->smbH.Ecode = bswap_16(RRsp->smbH.Ecode);

	// check there's no error
	if ((RRsp->smbH.Eclass | RRsp->smbH.Ecode) != STATUS_SUCCESS) {
		smb_printf("Eclass: %08x\n Ecode: %08x\n", RRsp->smbH.Eclass, RRsp->smbH.Ecode);
		return -2;
	}

	r = bswap_16(RRsp->DataLengthLow);

	if (bswap_16(RRsp->DataOffset) > 0)
		memcpy(readbuf, &SMB_buf[4 + bswap_16(RRsp->DataOffset)], r);

	return r;
}

//-------------------------------------------------------------------------

int smb_WriteAndX(int UID, int TID, int FID, s64 fileoffset, void *writebuf, u16 nbytes) {
	register int r;
	struct WriteAndXRequest_t *WR = (struct WriteAndXRequest_t *) SMB_buf;

	memcpy(WR, &smb_Write_Request.smbH.sessionHeader, sizeof (struct WriteAndXRequest_t));

	WR->smbH.UID = bswap_16((u16) UID);
	WR->smbH.TID = bswap_16((u16) TID);
	WR->FID = bswap_16((u16) FID);
	WR->OffsetLow = bswap_32((u32) (fileoffset & 0xffffffff));
	WR->OffsetHigh = bswap_32((u32) ((fileoffset >> 32) & 0xffffffff));
	WR->Remaining = bswap_16(nbytes);
	WR->DataLengthLow = bswap_16(nbytes);
	WR->ByteCount = bswap_16(nbytes);

	memcpy((void *) (&SMB_buf[4 + WR->DataOffset]), writebuf, nbytes);
	
	WR->DataOffset = bswap_16(WR->DataOffset);

	rawTCP_SetSessionHeader(63 + nbytes);
	r = GetSMBServerReply();
	if (r <= 0)
		return -3;

	struct WriteAndXResponse_t *WRsp = (struct WriteAndXResponse_t *) SMB_buf;

	// check sanity of SMB header
	if (WRsp->smbH.Magic != SMB_MAGIC)
		return -1;

	WRsp->smbH.Eclass = bswap_16(WRsp->smbH.Eclass);
	WRsp->smbH.Ecode = bswap_16(WRsp->smbH.Ecode);

	// check there's no error
	if ((WRsp->smbH.Eclass | WRsp->smbH.Ecode) != STATUS_SUCCESS) {
		smb_printf("Eclass: %08x\n Ecode: %08x\n", WRsp->smbH.Eclass, WRsp->smbH.Ecode);
		return -2;
	}

	return nbytes;
}

//-------------------------------------------------------------------------

int smb_Close(int UID, int TID, int FID) {
	register int r;
	struct CloseRequest_t *CR = (struct CloseRequest_t *) SMB_buf;

	memset(SMB_buf, 0, sizeof (SMB_buf));

	CR->smbH.Magic = SMB_MAGIC;
	CR->smbH.Cmd = SMB_COM_CLOSE;
	CR->smbH.Flags = SMB_FLAGS_CANONICAL_PATHNAMES; //| SMB_FLAGS_CASELESS_PATHNAMES;
	CR->smbH.Flags2 = bswap_16(SMB_FLAGS2_KNOWS_LONG_NAMES);
	CR->smbH.UID = bswap_16((u16) UID);
	CR->smbH.TID = bswap_16((u16) TID);
	CR->smbWordcount = 3;
	CR->FID = bswap_16((u16) FID);

	rawTCP_SetSessionHeader(41);
	r = GetSMBServerReply();
	if (r <= 0)
		return -3;

	struct CloseResponse_t *CRsp = (struct CloseResponse_t *) SMB_buf;

	// check sanity of SMB header
	if (CRsp->smbH.Magic != SMB_MAGIC) {
		smb_printf("Wrong magic in response.\n");
		return -1;
	}

	CRsp->smbH.Eclass = bswap_16(CRsp->smbH.Eclass);
	CRsp->smbH.Ecode = bswap_16(CRsp->smbH.Ecode);


	// check there's no error
	if ((CRsp->smbH.Eclass | CRsp->smbH.Ecode) != STATUS_SUCCESS) {
		smb_printf("Eclass: %08x\n Ecode: %08x\n", CRsp->smbH.Eclass, CRsp->smbH.Ecode);
		return -2;
	}

	return 0;
}

//-------------------------------------------------------------------------

int smb_Delete(int UID, int TID, char *Path) {
	register int r, CF, PathLen, i;
	struct DeleteRequest_t *DR = (struct DeleteRequest_t *) SMB_buf;

	memset(SMB_buf, 0, sizeof (SMB_buf));

	CF = server_specs.StringsCF;

	DR->smbH.Magic = SMB_MAGIC;
	DR->smbH.Cmd = SMB_COM_DELETE;
	DR->smbH.Flags = SMB_FLAGS_CANONICAL_PATHNAMES; //| SMB_FLAGS_CASELESS_PATHNAMES;
	DR->smbH.Flags2 = SMB_FLAGS2_KNOWS_LONG_NAMES;
	if (CF == 2)
		DR->smbH.Flags2 |= SMB_FLAGS2_UNICODE_STRING;

	DR->smbH.Flags2 = bswap_16(DR->smbH.Flags2);
	DR->smbH.UID = bswap_16((u16) UID);
	DR->smbH.TID = bswap_16((u16) TID);
	DR->smbWordcount = 1;
	DR->SearchAttributes = 0; // coud be other attributes to find Hidden/System files
	DR->BufferFormat = 0x04;

	PathLen = 0;
	for (i = 0; i < strlen(Path); i++) {
		DR->FileName[PathLen] = Path[i]; // add Path
		PathLen += CF;
	}
	PathLen += CF; // null terminator

	DR->ByteCount = bswap_16(PathLen + 1); // +1 for the BufferFormat byte

	rawTCP_SetSessionHeader(38 + PathLen);
	r = GetSMBServerReply();
	if (r <= 0)
		return -3;

	struct DeleteResponse_t *DRsp = (struct DeleteResponse_t *) SMB_buf;

	// check sanity of SMB header
	if (DRsp->smbH.Magic != SMB_MAGIC)
		return -1;

	DRsp->smbH.Eclass = bswap_16(DRsp->smbH.Eclass);
	DRsp->smbH.Ecode = bswap_16(DRsp->smbH.Ecode);

	// check there's no error
	if ((DRsp->smbH.Eclass | DRsp->smbH.Ecode) != STATUS_SUCCESS) {
		smb_printf("Eclass: %08x\n Ecode: %08x\n", DRsp->smbH.Eclass, DRsp->smbH.Ecode);
		return -2;
	}

	return 0;
}

//-------------------------------------------------------------------------

int smb_ManageDirectory(int UID, int TID, char *Path, int cmd) {
	register int r, CF, PathLen, i;
	struct ManageDirectoryRequest_t *MDR = (struct ManageDirectoryRequest_t *) SMB_buf;

	memset(SMB_buf, 0, sizeof (SMB_buf));

	CF = server_specs.StringsCF;

	MDR->smbH.Magic = SMB_MAGIC;

	MDR->smbH.Cmd = (u8) cmd;
	MDR->smbH.Flags = SMB_FLAGS_CANONICAL_PATHNAMES; //| SMB_FLAGS_CASELESS_PATHNAMES;
	MDR->smbH.Flags2 = SMB_FLAGS2_KNOWS_LONG_NAMES;
	if (CF == 2)
		MDR->smbH.Flags2 |= SMB_FLAGS2_UNICODE_STRING;
	MDR->smbH.Flags2 = bswap_16(MDR->smbH.Flags2);
	MDR->smbH.UID = bswap_16((u16) UID);
	MDR->smbH.TID = bswap_16((u16) TID);
	MDR->BufferFormat = 0x04;

	PathLen = 0;
	for (i = 0; i < strlen(Path); i++) {
		MDR->DirectoryName[PathLen] = Path[i]; // add Path
		PathLen += CF;
	}
	PathLen += CF; // null terminator

	MDR->ByteCount = bswap_16(PathLen + 1); // +1 for the BufferFormat byte

	rawTCP_SetSessionHeader(36 + PathLen);
	r = GetSMBServerReply();
	if (r <= 0)
		return -3;

	struct ManageDirectoryResponse_t *MDRsp = (struct ManageDirectoryResponse_t *) SMB_buf;

	// check sanity of SMB header
	if (MDRsp->smbH.Magic != SMB_MAGIC)
		return -1;

	MDRsp->smbH.Eclass = bswap_16(MDRsp->smbH.Eclass);
	MDRsp->smbH.Ecode = bswap_16(MDRsp->smbH.Ecode);

	// check there's no error
	if ((MDRsp->smbH.Eclass | MDRsp->smbH.Ecode) != STATUS_SUCCESS) {
		smb_printf("Eclass: %08x\n Ecode: %08x\n", MDRsp->smbH.Eclass, MDRsp->smbH.Ecode);
		return -2;
	}

	return 0;
}

//-------------------------------------------------------------------------

int smb_Rename(int UID, int TID, char *oldPath, char *newPath) {
	register int r, CF, offset, i;
	struct RenameRequest_t *RR = (struct RenameRequest_t *) SMB_buf;

	memset(SMB_buf, 0, sizeof (SMB_buf));

	CF = server_specs.StringsCF;

	RR->smbH.Magic = SMB_MAGIC;
	RR->smbH.Cmd = SMB_COM_RENAME;
	RR->smbH.Flags = SMB_FLAGS_CANONICAL_PATHNAMES; //| SMB_FLAGS_CASELESS_PATHNAMES;
	RR->smbH.Flags2 = SMB_FLAGS2_KNOWS_LONG_NAMES;
	if (CF == 2)
		RR->smbH.Flags2 |= SMB_FLAGS2_UNICODE_STRING;

	RR->smbH.Flags2 = bswap_16(RR->smbH.Flags2);

	RR->smbH.UID = bswap_16((u16) UID);
	RR->smbH.TID = bswap_16((u16) TID);
	RR->smbWordcount = 1;

	// NOTE: on samba seems it doesn't care of attribute to rename directories
	// to be tested on windows 
	RR->SearchAttributes = 0; // coud be other attributes to find Hidden/System files /Directories

	offset = 0;
	RR->ByteField[offset++] = 0x04; // BufferFormat

	for (i = 0; i < strlen(oldPath); i++) {
		RR->ByteField[offset] = oldPath[i]; // add oldPath
		offset += CF;
	}
	offset += CF; // null terminator

	RR->ByteField[offset++] = 0x04; // BufferFormat

	if (CF == 2)
		offset++; // pad needed for unicode

	for (i = 0; i < strlen(newPath); i++) {
		RR->ByteField[offset] = newPath[i]; // add newPath
		offset += CF;
	}
	offset += CF; // null terminator

	RR->ByteCount = bswap_16(offset);

	rawTCP_SetSessionHeader(37 + offset);
	r = GetSMBServerReply();
	if (r <= 0)
		return -3;

	struct RenameResponse_t *RRsp = (struct RenameResponse_t *) SMB_buf;

	// check sanity of SMB header
	if (RRsp->smbH.Magic != SMB_MAGIC)
		return -1;

	RRsp->smbH.Eclass = bswap_16(RRsp->smbH.Eclass);
	RRsp->smbH.Ecode = bswap_16(RRsp->smbH.Ecode);

	// check there's no error
	if ((RRsp->smbH.Eclass | RRsp->smbH.Ecode) != STATUS_SUCCESS) {
		smb_printf("Eclass: %08x\n Ecode: %08x\n", RRsp->smbH.Eclass, RRsp->smbH.Ecode);
		return -2;
	}

	return 0;
}

//-------------------------------------------------------------------------

int smb_FindFirstNext2(int UID, int TID, char *Path, int cmd, SearchInfo_t *info) {
	register int r, CF, PathLen, i, j;
	struct FindFirstNext2Request_t *FFNR = (struct FindFirstNext2Request_t *) SMB_buf;

	memset(SMB_buf, 0, sizeof (SMB_buf));

	CF = server_specs.StringsCF;

	FFNR->smbH.Magic = SMB_MAGIC;
	FFNR->smbH.Cmd = SMB_COM_TRANSACTION2;
	FFNR->smbH.Flags = SMB_FLAGS_CANONICAL_PATHNAMES; //| SMB_FLAGS_CASELESS_PATHNAMES;
	FFNR->smbH.Flags2 = SMB_FLAGS2_KNOWS_LONG_NAMES;
	if (CF == 2)
		FFNR->smbH.Flags2 |= SMB_FLAGS2_UNICODE_STRING;

	FFNR->smbH.Flags2 = bswap_16(FFNR->smbH.Flags2);

	FFNR->smbH.UID = bswap_16((u16) UID);
	FFNR->smbH.TID = bswap_16((u16) TID);
	FFNR->smbWordcount = 15;

	FFNR->smbTrans.SetupCount = 1;
	FFNR->SubCommand = (u8) cmd;

	FFNR->smbTrans.ParamOffset = 68;
	FFNR->smbTrans.MaxParamCount = 256; // Max Parameters len in reply
	FFNR->smbTrans.MaxDataCount = 16384; // Max Data len in reply

	if (cmd == TRANS2_FIND_FIRST2) {
		struct FindFirst2RequestParam_t *FFRParam = (struct FindFirst2RequestParam_t *) &SMB_buf[FFNR->smbTrans.ParamOffset + 4];

		FFRParam->SearchAttributes = bswap_16(ATTR_READONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_DIRECTORY | ATTR_ARCHIVE);
		FFRParam->SearchCount = bswap_16(1);
		FFRParam->Flags = bswap_16(CLOSE_SEARCH_IF_EOS | RESUME_SEARCH);
		FFRParam->LevelOfInterest = bswap_16(SMB_FIND_FILE_BOTH_DIRECTORY_INFO);

		PathLen = 0;
		for (i = 0; i < strlen(Path); i++) {
			FFRParam->SearchPattern[PathLen] = Path[i]; // add Path
			PathLen += CF;
		}
		PathLen += CF; // null terminator

		FFNR->smbTrans.TotalParamCount = FFNR->smbTrans.ParamCount = 2 + 2 + 2 + 2 + 4 + PathLen;
	} else {
		struct FindNext2RequestParam_t *FNRParam = (struct FindNext2RequestParam_t *) &SMB_buf[FFNR->smbTrans.ParamOffset + 4];

		FNRParam->SearchID = bswap_16((u16) info->SID);
		FNRParam->SearchCount = bswap_16(1);
		FNRParam->LevelOfInterest = bswap_16(SMB_FIND_FILE_BOTH_DIRECTORY_INFO);
		FNRParam->Flags = bswap_16(CLOSE_SEARCH_IF_EOS | RESUME_SEARCH | CONTINUE_SEARCH);
		FNRParam->SearchPattern[0] = 0;
		FFNR->smbTrans.TotalParamCount = FFNR->smbTrans.ParamCount = 2 + 2 + 2 + 2 + 4 + 1;
	}

	FFNR->ByteCount = 3 + FFNR->smbTrans.TotalParamCount;
	FFNR->smbTrans.DataOffset = FFNR->smbTrans.ParamOffset + FFNR->smbTrans.TotalParamCount;

	rawTCP_SetSessionHeader(FFNR->smbTrans.DataOffset);
	
	// bswap
	FFNR->SubCommand = bswap_16(FFNR->SubCommand);
	FFNR->ByteCount = bswap_16(FFNR->ByteCount);
		
	FFNR->smbTrans.DataCount = bswap_16(FFNR->smbTrans.DataCount);
	FFNR->smbTrans.DataOffset = bswap_16(FFNR->smbTrans.DataOffset);
	FFNR->smbTrans.MaxDataCount = bswap_16(FFNR->smbTrans.MaxDataCount);
	FFNR->smbTrans.MaxParamCount = bswap_16(FFNR->smbTrans.MaxParamCount);
	FFNR->smbTrans.ParamCount = bswap_16(FFNR->smbTrans.ParamCount);
	FFNR->smbTrans.ParamOffset = bswap_16(FFNR->smbTrans.ParamOffset);
	FFNR->smbTrans.TotalParamCount = bswap_16(FFNR->smbTrans.TotalParamCount);

	r = GetSMBServerReply();
	if (r <= 0)
		return -3;

	struct FindFirstNext2Response_t *FFNRsp = (struct FindFirstNext2Response_t *) SMB_buf;

	// check sanity of SMB header
	if (FFNRsp->smbH.Magic != SMB_MAGIC)
		return -1;

	FFNRsp->smbH.Eclass = bswap_16(FFNRsp->smbH.Eclass);
	FFNRsp->smbH.Ecode = bswap_16(FFNRsp->smbH.Ecode);

	// check there's no error
	if ((FFNRsp->smbH.Eclass | FFNRsp->smbH.Ecode) != STATUS_SUCCESS) {
		smb_printf("Error: %04x%04x\n", FFNRsp->smbH.Ecode & 0xFFFF, FFNRsp->smbH.Eclass & 0xFFFF);
		return -2;
	}

	struct FindFirstNext2ResponseParam_t *FFNRspParam;

	if (cmd == TRANS2_FIND_FIRST2) {
		FFNRspParam = (struct FindFirstNext2ResponseParam_t *) &SMB_buf[bswap_16(FFNRsp->smbTrans.ParamOffset) + 4];
		info->SID = bswap_16(FFNRspParam->SearchID);
	} else
		FFNRspParam = (struct FindFirstNext2ResponseParam_t *) &SMB_buf[bswap_16(FFNRsp->smbTrans.ParamOffset) + 4 - 2];

	struct FindFirst2ResponseData_t *FFRspData = (struct FindFirst2ResponseData_t *) &SMB_buf[bswap_16(FFNRsp->smbTrans.DataOffset) + 4];

	info->EOS = bswap_16(FFNRspParam->EndOfSearch);

	if (FFNRspParam->SearchCount == 0) {
		return -4;
	}

	info->fileInfo.Created = bswap_64(FFRspData->Created);
	info->fileInfo.LastAccess = bswap_64(FFRspData->LastAccess);
	info->fileInfo.LastWrite = bswap_64(FFRspData->LastWrite);
	info->fileInfo.Change = bswap_64(FFRspData->Change);
	info->fileInfo.FileAttributes = bswap_32(FFRspData->FileAttributes);
	if (FFRspData->FileAttributes & EXT_ATTR_DIRECTORY)
		info->fileInfo.IsDirectory = 1;
	info->fileInfo.AllocationSize = bswap_64(FFRspData->AllocationSize);
	info->fileInfo.EndOfFile = bswap_64(FFRspData->EndOfFile);

	FFRspData->FileNameLen = bswap_32(FFRspData->FileNameLen);

	for (i = 0, j = 0; i < FFRspData->FileNameLen; i++, j += CF)
		info->FileName[i] = FFRspData->FileName[j];

	return 0;
}

//-------------------------------------------------------------------------

int smb_TreeDisconnect(int UID, int TID) {
	register int r;
	struct TreeDisconnectRequest_t *TDR = (struct TreeDisconnectRequest_t *) SMB_buf;

	memset(SMB_buf, 0, sizeof (SMB_buf));

	TDR->smbH.Magic = SMB_MAGIC;
	TDR->smbH.Cmd = SMB_COM_TREE_DISCONNECT;
	TDR->smbH.UID = bswap_16((u16) UID);
	TDR->smbH.TID = bswap_16((u16) TID);

	rawTCP_SetSessionHeader(35);
	r = GetSMBServerReply();
	if (r <= 0)
		return -3;

	struct TreeDisconnectResponse_t *TDRsp = (struct TreeDisconnectResponse_t *) SMB_buf;

	// check sanity of SMB header
	if (TDRsp->smbH.Magic != SMB_MAGIC)
		return -1;

	TDRsp->smbH.Eclass = bswap_16(TDRsp->smbH.Eclass);
	TDRsp->smbH.Ecode = bswap_16(TDRsp->smbH.Ecode);

	// check there's no error
	if ((TDRsp->smbH.Eclass | TDRsp->smbH.Ecode) != STATUS_SUCCESS) {
		smb_printf("Eclass: %08x\n Ecode: %08x\n", TDRsp->smbH.Eclass, TDRsp->smbH.Ecode);
		return -2;
	}

	TID = -1;

	return 0;
}

//-------------------------------------------------------------------------

int smb_LogOffAndX(int UID) {
	register int r;
	struct LogOffAndXRequest_t *LR = (struct LogOffAndXRequest_t *) SMB_buf;

	memset(SMB_buf, 0, sizeof (SMB_buf));

	LR->smbH.Magic = SMB_MAGIC;
	LR->smbH.Cmd = SMB_COM_LOGOFF_ANDX;
	LR->smbH.UID = bswap_16((u16) UID);
	LR->smbWordcount = 2;
	LR->smbAndxCmd = SMB_COM_NONE; // no ANDX command

	rawTCP_SetSessionHeader(39);
	r = GetSMBServerReply();
	if (r <= 0)
		return -3;

	struct LogOffAndXResponse_t *LRsp = (struct LogOffAndXResponse_t *) SMB_buf;

	// check sanity of SMB header
	if (LRsp->smbH.Magic != SMB_MAGIC)
		return -1;

	LRsp->smbH.Eclass = bswap_16(LRsp->smbH.Eclass);
	LRsp->smbH.Ecode = bswap_16(LRsp->smbH.Ecode);

	// check there's no error
	if ((LRsp->smbH.Eclass | LRsp->smbH.Ecode) != STATUS_SUCCESS) {
		smb_printf("Eclass: %08x\n Ecode: %08x\n", LRsp->smbH.Eclass, LRsp->smbH.Ecode);
		return -2;
	}

	UID = -1;

	return 0;
}

//-------------------------------------------------------------------------

int smb_Echo(void *echo, int len) {
	register int r;
	struct EchoRequest_t *ER = (struct EchoRequest_t *) SMB_buf;

	memset(SMB_buf, 0, sizeof (SMB_buf));

	ER->smbH.Magic = SMB_MAGIC;
	ER->smbH.Cmd = SMB_COM_ECHO;
	ER->smbWordcount = 1;
	ER->EchoCount = bswap_16(1);

	memcpy(&ER->ByteField[0], echo, (u16) len);
	ER->ByteCount = bswap_16((u16) len);

	rawTCP_SetSessionHeader(37 + (u16) len);
	r = GetSMBServerReply();
	if (r <= 0)
		return -4;

	struct EchoResponse_t *ERsp = (struct EchoResponse_t *) SMB_buf;

	// check sanity of SMB header
	if (ERsp->smbH.Magic != SMB_MAGIC)
		return -1;

	ERsp->smbH.Eclass = bswap_16(ERsp->smbH.Eclass);
	ERsp->smbH.Ecode = bswap_16(ERsp->smbH.Ecode);

	// check there's no error
	if ((ERsp->smbH.Eclass | ERsp->smbH.Ecode) != STATUS_SUCCESS) {
		smb_printf("Eclass: %08x\n Ecode: %08x\n", ERsp->smbH.Eclass, ERsp->smbH.Ecode);
		return -2;
	}

	if (memcmp(&ERsp->ByteField[0], echo, len))
		return -3;

	return 0;
}

//-------------------------------------------------------------------------

int smb_Connect(char *SMBServerIP, int SMBServerPort) {
	register int r, retry_count = 0;
	struct in_addr dst_addr;

	dst_addr.s_addr = inet_addr(SMBServerIP);

conn_retry:

	// Close the connection if it was already opened
	smb_Disconnect();

	// Opening TCP session
	r = OpenTCPSession(dst_addr, SMBServerPort, &main_socket);
	if (r < 0) {
		retry_count++;
		if (retry_count < 3)
			goto conn_retry;
		return -1;
	}

	// We keep the server IP for SMB logon
	strncpy(server_specs.ServerIP, SMBServerIP, 16);

	return 0;
}

//-------------------------------------------------------------------------

int smb_Disconnect(void) {
	if (main_socket != -1) {
		lwip_close(main_socket);
		main_socket = -1;
	}

	return 0;
}
