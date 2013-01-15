/**************************************************************************
*            Unix-like crypt(3) Algorithm for Password Encryption
*
*   File    : crypt3.c
*   Purpose : Provides crypt(3) functionality to ANSI C compilers
*             without a need for the crypt library.
*   Author  : Michael Dipperstein
*   Date    : November 3, 1998
*
***************************************************************************
*   The source in this file is heavily borrowed from the crypt3.c file
*   found on several ftp sites on the Internet.  The original source
*   claimed to be BSD, but was not distributed with any BSD license or
*   copyright claims.  I am releasing the source that I have provided into
*   public domain without any restrictions, warranties, or copyright
*   claims of my own.
*
*   The code below has been cleaned and compiles correctly under, gcc,
*   lcc, and Borland's bcc C compilers.  A bug involving the left and
*   right halves of the encrypted data block in the widely published
*   crypt3.c source has been fixed by this version.  All implicit register
*   declarations have been removed, because they generated suboptimal code.
*   All constant data has been explicitly declared as const and all
*   declarations have been given a minimal scope, because I'm paranoid.
*
*   Caution: crypt() returns a pointer to static data.  I left it this way
*            to maintain backward compatibility.  The downside is that
*            successive calls will cause previous results to be lost.
*            This can easily be changed with only minor modifications to
*            the function crypt().
**************************************************************************/

/**************************************************************************
* Function:    crypt
*
* Description: Clone of Unix crypt(3) function.
*
* Inputs:      char *pw
*              pointer to 8 character encryption key (user password)
*              char *salt
*              pointer to 2 character salt used to modify the DES results.
*
* Returns:     Pointer to static array containing the salt concatenated
*              on to the encrypted results.  Same as stored in passwd file.
**************************************************************************/
char *crypt(char *pw, char *salt);
