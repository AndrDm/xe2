        ��  ��                  �       �� ��     0 	        <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0" />�      �� ��     0 	        �4   V S _ V E R S I O N _ I N F O     ���                 ?                             S t r i n g F i l e I n f o   �   0 4 0 9 0 4 b 0        C o m p a n y N a m e     X   F i l e D e s c r i p t i o n     M P   H e l p e r   ( R e l e a s e   x 6 4 )   (   F i l e V e r s i o n     1 . 0   4 
  I n t e r n a l N a m e   M P   H e l p e r   F   L e g a l C o p y r i g h t   C o p y r i g h t       2 0 2 3     D   O r i g i n a l F i l e n a m e   M P   H e l p e r . d l l   6   P r o d u c t N a m e       M P   H e l p e r     ,   P r o d u c t V e r s i o n   1 . 0   D    V a r F i l e I n f o     $    T r a n s l a t i o n     	�z  X   C V I D L L P R O T O T Y P E S   C V I D L L E X P O R T S         0 	        struct Image_struct *ADV_LVDTToAddress(const void *LVImageHdl);
int ADV_SetLVError(int ErrorCode, const char *errText, struct LVErrorCluster *ErrorCluster);
int ADV_SetLVErrorWithCallChain(int ErrorCode, struct LVErrorCluster *ErrorCluster);
void ApplyPower(unsigned long long SrcImage, double Power, struct LVErrorCluster *ErrorCluster);
void ApplyTransform(unsigned long long SrcImage, int StartLine, int EndLine, double Divider, double Power, double Multiplier, struct LVErrorCluster *ErrorCluster);
int DllMain(struct HINSTANCE__ *hinstDLL, unsigned long fdwReason, void *lpvReserved);
inline double fastPow(double a, double b);
           �� ��     0	                                        