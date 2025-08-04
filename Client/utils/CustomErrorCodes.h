#ifndef CUSTOMERRORCODES_H
#define CUSTOMERRORCODES_H

enum CustomBaseErrorCodes {
    Custom_Base             = 0,
    Custom_Domain_Error     = 4,
    Custom_Repository_Error = 8,
    Custom_Service_Error    = 12,
    Custom_Controller_Error = 16,
    Custom_Util_Error       = 20,
    Custom_View_Error       = 24,
    Custom_Etc_Error        = 28,
    Custom_Max_Error        = 31,
};


enum RaErrorCode{
    // Common Error
    Ra_Success          = 1 << (Custom_Base+0),
    Ra_Unkown_Error     = 1 << (Custom_Base+1),

    // Domain Error
    Ra_Domain_Unkown_Error  = 1 << (Custom_Domain_Error+0),

    // Repository Error
    Ra_Repo_Unkown_Error  = 1 << (Custom_Repository_Error+0),

    // Service Error
    Ra_Service_Unkown_Error  = 1 << (Custom_Service_Error+0),

    // Controller Error
    Ra_Controller_Unkown_Error  = 1 << (Custom_Controller_Error+0),

    // Util Error
    Ra_Util_Unkown_Error  = 1 << (Custom_Util_Error+0),

    // View Error
    Ra_View_Unkown_Error  = 1 << (Custom_View_Error+0),

    // Etc Error
    Ra_Etc_Unkown_Error  = 1 << (Custom_Etc_Error+0),

};

#endif // CUSTOMERRORCODES_H
