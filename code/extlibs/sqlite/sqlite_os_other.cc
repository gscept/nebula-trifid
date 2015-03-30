
/**
    os_nebula3
    
    SQLite OS file for use with nebula3.
    Uses only io functions from nebula3.
    For multi plattform use.
        
    (C) 2007 Radon Labs GmbH
*/



#include "stdneb.h"
#include "io/ioserver.h"
#include "io/filestream.h"
#include "memory/memory.h"

extern "C"
{
#include "sqliteInt.h"
#include "sqlite_os.h"

#ifndef _NO_NEBULA_MEMALLOC
// overwrite memfuncs
void *sqlite3GenericMalloc(int n){
    void *p = Memory::Alloc(Memory::DefaultHeap, (size_t)n);    
    return p;
}
void *sqlite3GenericRealloc(void *p, int n){    
    return Memory::Realloc(Memory::DefaultHeap, p, n);
}
void sqlite3GenericFree(void *p){
    Memory::Free(Memory::DefaultHeap, p);
}
#endif

#if __WII__ 
typedef wchar_t WCHAR;
typedef OSTime FILETIME;
typedef OSTime LPSYSTEMTIME;
typedef long LONG;
#define SEEK(X)
#define TRACE1(X)
#define TRACE2(X,Y)
#define TRACE3(X,Y,Z)
#define TRACE4(X,Y,Z,A)
#define TRACE5(X,Y,Z,A,B)
#define TRACE6(X,Y,Z,A,B,C)
#define TRACE7(X,Y,Z,A,B,C,D)

#define SimulateIOError(A)
#define SimulateDiskfullError(A)
#define OpenCounter(X)

#define CP_UTF8  65001       // UTF-8 translation
#elif __PS3__
typedef long LONG;  
typedef wchar_t WCHAR;
typedef double FILETIME;
typedef double LPSYSTEMTIME;
#endif

#if !__WII__
/*
** Include code that is common to all os_*.c files
*/
#include "sqlite_os_common.h"
#endif

#ifdef __CYGWIN__
# include <sys/cygwin.h>
#endif

/*
** Macros used to determine whether or not to use threads.
*/
#if defined(THREADSAFE) && THREADSAFE
# define SQLITE_W32_THREADS 1
#endif

# define AreFileApisANSI() 1

/*
** The winFile structure is a subclass of OsFile specific to the win32
** portability layer.
*/
typedef struct winFile winFile;
struct winFile {
  IoMethod const *pMethod;/* Must be first */
  //HANDLE h;               /* Handle for accessing the file */
  IO::FileStream* stream;
  unsigned char locktype; /* Type of lock currently held on this file */
  short sharedLockByte;   /* Randomly chosen byte used as a shared lock */
};


/*
** Do not include any of the File I/O interface procedures if the
** SQLITE_OMIT_DISKIO macro is defined (indicating that there database
** will be in-memory only)
*/
#ifndef SQLITE_OMIT_DISKIO

/*
** The following variable is (normally) set once and never changes
** thereafter.  It records whether the operating system is Win95
** or WinNT.
**
** 0:   Operating system unknown.
** 1:   Operating system is Win95.
** 2:   Operating system is WinNT.
**
** In order to facilitate testing on a WinNT system, the test fixture
** can manually set this value to 1 to emulate Win98 behavior.
*/
int sqlite3_os_type = 0;

#if !__WII__  && !__PS3__
/*
** Convert a UTF-8 string to microsoft unicode (UTF-16?). 
**
** Space to hold the returned string is obtained from Memory::Alloc.
*/
static WCHAR *utf8ToUnicode(const char *zFilename){
  int nChar;
  WCHAR *zWideFilename;

  nChar = MultiByteToWideChar(CP_UTF8, 0, zFilename, -1, NULL, 0);
  zWideFilename = (WCHAR*)Memory::Alloc(Memory::DefaultHeap, nChar*sizeof(zWideFilename[0]) );
  if( zWideFilename==0 ){
    return 0;
  }
  nChar = MultiByteToWideChar(CP_UTF8, 0, zFilename, -1, zWideFilename, nChar);
  if( nChar==0 ){
      Memory::Free(Memory::DefaultHeap, zWideFilename);
    zWideFilename = 0;
  }
  return zWideFilename;
}

/*
** Convert microsoft unicode to multibyte character string, based on the
** user's Ansi codepage.
**
** Space to hold the returned string is obtained from
** Memory::Alloc().
*/
static char *unicodeToMbcs(const WCHAR *zWideFilename){
  int nByte;
  char *zFilename;
  int codepage = CP_ACP;

  nByte = WideCharToMultiByte(codepage, 0, zWideFilename, -1, 0, 0, 0, 0);
  zFilename = (char*)Memory::Alloc(Memory::DefaultHeap, nByte );
  if( zFilename==0 ){
    return 0;
  }
  nByte = WideCharToMultiByte(codepage, 0, zWideFilename, -1, zFilename, nByte,
                              0, 0);
  if( nByte == 0 ){
    Memory::Free(Memory::DefaultHeap, zFilename);
    zFilename = 0;
  }
  return zFilename;
}

/*
** Convert UTF-8 to multibyte character string.  Space to hold the 
** returned string is obtained from Memory::Alloc().
*/
static char *utf8ToMbcs(const char *zFilename){
  char *zFilenameMbcs;
  WCHAR *zTmpWide;

  zTmpWide = utf8ToUnicode(zFilename);
  if( zTmpWide==0 ){
    return 0;
  }
  zFilenameMbcs = unicodeToMbcs(zTmpWide);
  Memory::Free(Memory::DefaultHeap, zTmpWide);
  return zFilenameMbcs;
}

#endif
/*
** Convert a UTF-8 filename into whatever form the underlying
** operating system wants filenames in.  Space to hold the result
** is obtained from Memory::Alloc and must be freed by the calling
** function.
*/
static void *convertUtf8Filename(const char *zFilename){
  void *zConverted = 0;  
  #if !__WII__  && !__PS3__
    zConverted = utf8ToMbcs(zFilename);
    #else
    zConverted = (void*)sqlite3StrDup(zFilename);
  #endif  
  /* caller will handle out of memory */
  return zConverted;
}

/*
** Delete the named file.
**
** Note that windows does not allow a file to be deleted if some other
** process has it open.  Sometimes a virus scanner or indexing program
** will open a journal file shortly after it is created in order to do
** whatever it is it does.  While this other process is holding the
** file open, we will be unable to delete it.  To work around this
** problem, we delay 100 milliseconds and try to delete again.  Up
** to MX_DELETION_ATTEMPTs deletion attempts are run before giving
** up and returning an error.
*/
#define MX_DELETION_ATTEMPTS 3
int sqlite3Nebula3Delete(const char *zFilename)
{
    int cnt = 0;
    bool fileDeleted;
    void *zConverted = convertUtf8Filename(zFilename);
    if (zConverted == 0)
    {
      return SQLITE_NOMEM;
    }  
    do
    {
        fileDeleted = IO::IoServer::Instance()->DeleteFile((char*)zConverted);
    }
    while( !fileDeleted && cnt++ < MX_DELETION_ATTEMPTS && (Core::SysFunc::Sleep(0.1), 1) );

    Memory::Free(Memory::DefaultHeap, zConverted);
    TRACE2("DELETE \"%s\"\n", zFilename);
    return fileDeleted ? SQLITE_OK : SQLITE_IOERR;
}

/*
** Return TRUE if the named file exists.
*/
int sqlite3Nebula3FileExists(const char *zFilename){
  int exists = 0;
  void *zConverted = convertUtf8Filename(zFilename);
  if( zConverted==0 ){
    return SQLITE_NOMEM;
  }
  
  exists = IO::IoServer::Instance()->FileExists((char*)zConverted);

  Memory::Free(Memory::DefaultHeap, zConverted);
  return exists;
}

/* Forward declaration */
static int allocateNebula3File(IO::FileStream* stream, OsFile **pId);

/*
** Attempt to open a file for both reading and writing.  If that
** fails, try opening it read-only.  If the file does not exist,
** try to create it.
**
** On success, a handle for the open file is written to *id
** and *pReadonly is set to 0 if the file was opened for reading and
** writing or 1 if the file was opened read-only.  The function returns
** SQLITE_OK.
**
** On failure, the function returns SQLITE_CANTOPEN and leaves
** *id and *pReadonly unchanged.
*/
int sqlite3Nebula3OpenReadWrite(
  const char *zFilename,
  OsFile **pId,
  int *pReadonly
){
    void *zConverted = convertUtf8Filename(zFilename);
    if( zConverted==0 ){
    return SQLITE_NOMEM;
    }
    assert( *pId==0 );
    Ptr<IO::FileStream> fileStream;
    fileStream = IO::IoServer::Instance()->CreateStream((char*)zConverted).downcast<IO::FileStream>().get();
    fileStream->SetAccessMode(IO::Stream::ReadWriteAccess);
    fileStream->SetAccessPattern(IO::Stream::Random);
    fileStream->AddRef();

    // try open with read write access
    if (!fileStream->Open())
    {      
      // failed, try open with read only access
      fileStream->SetAccessMode(IO::Stream::ReadAccess);
      if (fileStream->Open())
      {         
          *pReadonly = 1;
      }
      else
      {
          // failed, can't open or create file
          Memory::Free(Memory::DefaultHeap, zConverted);
          return SQLITE_CANTOPEN;
      }
    }
    else
    {
        *pReadonly = 0;
    }
    Memory::Free(Memory::DefaultHeap, zConverted);
    
    return allocateNebula3File(fileStream, pId);
}


/*
** Attempt to open a new file for exclusive access by this process.
** The file will be opened for both reading and writing.  To avoid
** a potential security problem, we do not allow the file to have
** previously existed.  Nor do we allow the file to be a symbolic
** link.
**
** If delFlag is true, then make arrangements to automatically delete
** the file when it is closed.
**
** On success, write the file handle into *id and return SQLITE_OK.
**
** On failure, return SQLITE_CANTOPEN.
**
** Sometimes if we have just deleted a prior journal file, windows
** will fail to open a new one because there is a "pending delete".
** To work around this bug, we pause for 100 milliseconds and attempt
** a second open after the first one fails.  The whole operation only
** fails if both open attempts are unsuccessful.
*/
int sqlite3Nebula3OpenExclusive(const char *zFilename, OsFile **pId, int delFlag)
{
    void *zConverted = convertUtf8Filename(zFilename);
    if( zConverted==0 )
    {
        return SQLITE_NOMEM;
    }
    
    bool fileCreated = false;
    assert( *pId == 0 );
    
    Ptr<IO::FileStream> fileStream = IO::IoServer::Instance()->CreateStream((char*)zConverted).downcast<IO::FileStream>();
    fileStream->SetAccessPattern(IO::Stream::Random);
    fileStream->SetAccessMode(IO::Stream::ReadWriteAccess);
    fileStream->AddRef();

    {
        int cnt = 0;
        do
        { 
            fileCreated = fileStream->Open();       
        }
        while( !fileCreated && cnt++ < 2 && (Core::SysFunc::Sleep(0.1), 1) );
    }
    Memory::Free(Memory::DefaultHeap, zConverted);
    if( !fileCreated )
    {        
        return SQLITE_CANTOPEN;
    }    
        
    return allocateNebula3File(fileStream, pId);
}

/*
** Attempt to open a new file for read-only access.
**
** On success, write the file handle into *id and return SQLITE_OK.
**
** On failure, return SQLITE_CANTOPEN.
*/
int sqlite3Nebula3OpenReadOnly(const char *zFilename, OsFile **pId)
{  
    void *zConverted = convertUtf8Filename(zFilename);
    if( zConverted==0 )
    {
        return SQLITE_NOMEM;
    }
    assert( *pId==0 );    
    
    Ptr<IO::FileStream> fileStream = IO::IoServer::Instance()->CreateStream((char*)zConverted).downcast<IO::FileStream>();
    fileStream->SetAccessPattern(IO::Stream::Random);
    fileStream->SetAccessMode(IO::Stream::ReadAccess);
    fileStream->AddRef();
    
    if (!fileStream->Open())
    {
        Memory::Free(Memory::DefaultHeap, zConverted);
        return SQLITE_CANTOPEN;
    }
    Memory::Free(Memory::DefaultHeap, zConverted);
    
    return allocateNebula3File(fileStream, pId);
}

/*
** Attempt to open a file descriptor for the directory that contains a
** file.  This file descriptor can be used to fsync() the directory
** in order to make sure the creation of a new file is actually written
** to disk.
**
** This routine is only meaningful for Unix.  It is a no-op under
** windows since windows does not support hard links.
**
** On success, a handle for a previously open file is at *id is
** updated with the new directory file descriptor and SQLITE_OK is
** returned.
**
** On failure, the function returns SQLITE_CANTOPEN and leaves
** *id unchanged.
*/
static int winOpenDirectory(
  OsFile *id,
  const char *zDirname
){
  return SQLITE_OK;
}

/*
** If the following global variable points to a string which is the
** name of a directory, then that directory will be used to store
** temporary files.
*/
char *sqlite3_temp_directory = 0;

/*
** Create a temporary file name in zBuf.  zBuf must be big enough to
** hold at least SQLITE_TEMPNAME_SIZE characters.
*/
int sqlite3Nebula3TempFileName(char *zBuf){
  static char zChars[] =
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789";
  int i, j;
  char zTempPath[SQLITE_TEMPNAME_SIZE];
  if( sqlite3_temp_directory ){
    strncpy(zTempPath, sqlite3_temp_directory, SQLITE_TEMPNAME_SIZE-30);
    zTempPath[SQLITE_TEMPNAME_SIZE-30] = 0;
  }else {
      return SQLITE_NOMEM;    
  }
  for(i=strlen(zTempPath); i>0 && zTempPath[i-1]=='\\'; i--){}
  zTempPath[i] = 0;
  for(;;){
    sprintf(zBuf, "%s\\"TEMP_FILE_PREFIX, zTempPath);
    j = strlen(zBuf);
    sqlite3Randomness(15, &zBuf[j]);
    for(i=0; i<15; i++, j++){
      zBuf[j] = (char)zChars[ ((unsigned char)zBuf[j])%(sizeof(zChars)-1) ];
    }
    zBuf[j] = 0;
    if( !sqlite3OsFileExists(zBuf) ) break;
  }
  TRACE2("TEMP FILENAME: %s\n", zBuf);
  return SQLITE_OK; 
}

/*
** Close a file.
**
** It is reported that an attempt to close a handle might sometimes
** fail.  This is a very unreasonable result, but windows is notorious
** for being unreasonable so I do not doubt that it might happen.  If
** the close fails, we pause for 100 milliseconds and try again.  As
** many as MX_CLOSE_ATTEMPT attempts to close the handle are made before
** giving up and returning an error.
*/
#define MX_CLOSE_ATTEMPT 3
static int winClose(OsFile **pId){
  winFile *pFile;
  bool fileClosed = false;
  if( pId && (pFile = (winFile*)*pId)!=0 ){
    int cnt = 0;    
    do
    {            
        pFile->stream->Close();  
        fileClosed = !pFile->stream->IsOpen();

        if (fileClosed)
        {
            pFile->stream->Release();
            pFile->stream = 0;
        }
    }
    while(!fileClosed && cnt++ < MX_CLOSE_ATTEMPT && (Core::SysFunc::Sleep(0.1), 1) );
    
    pFile->stream = 0;
    OpenCounter(-1);
    Memory::Free(Memory::DefaultHeap, pFile);
    *pId = 0;
  }
  return fileClosed ? SQLITE_OK : SQLITE_IOERR;
}

/*
** Read data from a file into a buffer.  Return SQLITE_OK if all
** bytes were read successfully and SQLITE_IOERR if anything goes
** wrong.
*/
static int winRead(OsFile *id, void *pBuf, int amt){
  assert( id!=0 );
  SimulateIOError(return SQLITE_IOERR_READ);
  
  //if( !ReadFile(((winFile*)id)->h, pBuf, amt, &got, 0) ){
  ((winFile*)id)->stream->Read(pBuf, amt);
  
  return SQLITE_OK;
}

/*
** Write data from a buffer into a file.  Return SQLITE_OK on success
** or some other error code on failure.
*/
static int winWrite(OsFile *id, const void *pBuf, int amt){
  assert( id!=0 );
  SimulateIOError(return SQLITE_IOERR_READ);
  SimulateDiskfullError(return SQLITE_FULL);
  
  assert( amt>0 );  
  ((winFile*)id)->stream->Write(pBuf, amt);
   
  return SQLITE_OK;
}

/*
** Some microsoft compilers lack this definition.
*/
#ifndef INVALID_SET_FILE_POINTER
# define INVALID_SET_FILE_POINTER ((DWORD)-1)
#endif

/*
** Move the read/write pointer in a file.
*/
static int winSeek(OsFile *id, i64 offset)
{
    LONG upperBits = offset>>32;    
    n_assert2(upperBits == 0, "64bit file accessing not supported, database is too big!");
    assert( id!=0 );

#ifdef SQLITE_TEST
    if( offset ) SimulateDiskfullError(return SQLITE_FULL);
#endif

    SEEK(offset/1024 + 1);  
    ((winFile*)id)->stream->Seek(offset, IO::Stream::Begin);

    return SQLITE_OK;
}

/*
** Make sure all writes to a particular file are committed to disk.
*/
static int winSync(OsFile *id, int dataOnly)
{
    assert( id!=0 );   
    ((winFile*)id)->stream->Flush();
    return SQLITE_OK;    
}

/*
** Sync the directory zDirname. This is a no-op on operating systems other
** than UNIX.
*/
int sqlite3Nebula3SyncDirectory(const char *zDirname){
  SimulateIOError(return SQLITE_IOERR_READ);
  return SQLITE_OK;
}

/*
** Truncate an open file to a specified size
*/
static int winTruncate(OsFile *id, i64 nByte){
  LONG upperBits = nByte>>32;
    n_assert2(upperBits == 0, "64bit file accessing not supported, database is too big!");
  assert( id!=0 );
  //TRACE3("TRUNCATE %d %lld\n", ((winFile*)id)->h, nByte);
  SimulateIOError(return SQLITE_IOERR_TRUNCATE);
  ((winFile*)id)->stream->Seek(nByte, IO::Stream::Begin);
  //((winFile*)id)->stream->
  //SetFilePointer(((winFile*)id)->h, nByte, &upperBits, FILE_BEGIN);
  //SetEndOfFile(((winFile*)id)->h);
  return SQLITE_OK;
}

/*
** Determine the current size of a file in bytes
*/
static int winFileSize(OsFile *id, i64 *pSize){  
  assert( id!=0 );
  SimulateIOError(return SQLITE_IOERR_FSTAT);  
  *pSize = ((winFile*)id)->stream->GetSize();  
  return SQLITE_OK;
}

#ifndef SQLITE_OMIT_PAGER_PRAGMAS
/*
** Check that a given pathname is a directory and is writable 
**
*/
int sqlite3Nebula3IsDirWritable(char *zDirname)
{
  void *zConverted;
  if( zDirname==0 ) return 0;
  //if(strlen(zDirname)>MAX_PATH ) return 0;

  zConverted = convertUtf8Filename(zDirname);
  if( zConverted==0 ){
    return SQLITE_NOMEM;
  }
  // just check if dir exists
  return IO::IoServer::Instance()->DirectoryExists((char*)zConverted);

  //fileAttr = GetFileAttributesA((char*)zConverted);
  //
  //Memory::Free(zConverted);
  //if( fileAttr == 0xffffffff ) return 0;
  //if( (fileAttr & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY ){
  //  return 0;
  //}
  //return 1;
}
#endif /* SQLITE_OMIT_PAGER_PRAGMAS */

/*
** Lock the file with the lock specified by parameter locktype - one
** of the following:
**
**     (1) SHARED_LOCK
**     (2) RESERVED_LOCK
**     (3) PENDING_LOCK
**     (4) EXCLUSIVE_LOCK
**
** Sometimes when requesting one lock state, additional lock states
** are inserted in between.  The locking might fail on one of the later
** transitions leaving the lock state different from what it started but
** still short of its goal.  The following chart shows the allowed
** transitions and the inserted intermediate states:
**
**    UNLOCKED -> SHARED
**    SHARED -> RESERVED
**    SHARED -> (PENDING) -> EXCLUSIVE
**    RESERVED -> (PENDING) -> EXCLUSIVE
**    PENDING -> EXCLUSIVE
**
** This routine will only increase a lock.  The winUnlock() routine
** erases all locks at once and returns us immediately to locking level 0.
** It is not possible to lower the locking level one step at a time.  You
** must go straight to locking level 0.
*/
static int winLock(OsFile *id, int locktype){
    ((winFile*)id)->locktype = locktype;
    return SQLITE_OK;
}

/*
** This routine checks if there is a RESERVED lock held on the specified
** file by this or any other process. If such a lock is held, return
** non-zero, otherwise zero.
*/
static int winCheckReservedLock(OsFile *id){
    return (((winFile*)id)->locktype==RESERVED_LOCK);
}

/*
** Lower the locking level on file descriptor id to locktype.  locktype
** must be either NO_LOCK or SHARED_LOCK.
**
** If the locking level of the file descriptor is already at or below
** the requested locking level, this routine is a no-op.
**
** It is not possible for this routine to fail if the second argument
** is NO_LOCK.  If the second argument is SHARED_LOCK then this routine
** might return SQLITE_IOERR;
*/
static int winUnlock(OsFile *id, int locktype){ 
    winFile* pFile = (winFile*)id;
    if(pFile->locktype !=NO_LOCK)
    {
        pFile->locktype = locktype;
    }
    return SQLITE_OK;
}

/*
** Turn a relative pathname into a full pathname.  Return a pointer
** to the full pathname stored in space obtained from Memory::Alloc().
** The calling function is responsible for freeing this space once it
** is no longer needed.
*/
char *sqlite3Nebula3FullPathname(const char *zRelative){
    /* nebula3 solves relative path names */
    char *zAbsolute;
    zAbsolute=(char*)Memory::Alloc(Memory::DefaultHeap, strlen(zRelative)+1 );
    strcpy(zAbsolute, zRelative);
    return zAbsolute;
}

/*
** The fullSync option is meaningless on windows.   This is a no-op.
*/
static void winSetFullSync(OsFile *id, int v){
  return;
}

/*
** Return the underlying file handle for an OsFile
*/
static int winFileHandle(OsFile *id){

  return 0;//(int)((winFile*)id)->h;
}

/*
** Return an integer that indices the type of lock currently held
** by this handle.  (Used for testing and analysis only.)
*/
static int winLockState(OsFile *id){
  return ((winFile*)id)->locktype;
}

/*
** This vector defines all the methods that can operate on an OsFile
** for win32.
*/
static const IoMethod sqlite3Nebula3IoMethod = {
  winClose,
  winOpenDirectory,
  winRead,
  winWrite,
  winSeek,
  winTruncate,
  winSync,
  winSetFullSync,
  winFileHandle,
  winFileSize,
  winLock,
  winUnlock,
  winLockState,
  winCheckReservedLock,
};

/*
** Allocate memory for an OsFile.  Initialize the new OsFile
** to the value given in pInit and return a pointer to the new
** OsFile.  If we run out of memory, close the file and return NULL.
*/
//static int allocateNebula3File(winFile *pInit, OsFile **pId){
static int allocateNebula3File(IO::FileStream* stream, OsFile **pId)
{   
    winFile *pNew;
    pNew = (winFile*)Memory::Alloc(Memory::DefaultHeap, sizeof(winFile) );
    if( pNew==0 )
    {
        stream->Close();
        stream->Release();
        stream = 0;
        *pId = 0;
        return SQLITE_NOMEM;
    }
    else
    {      
        pNew->pMethod = &sqlite3Nebula3IoMethod;
        pNew->stream = stream;
        pNew->locktype = NO_LOCK;
        pNew->sharedLockByte = 0;
        *pId = (OsFile*)pNew;
        OpenCounter(+1);
        return SQLITE_OK;
    }
}


#endif /* SQLITE_OMIT_DISKIO */
/***************************************************************************
** Everything above deals with file I/O.  Everything that follows deals
** with other miscellanous aspects of the operating system interface
****************************************************************************/

/*
** Get information to seed the random number generator.  The seed
** is written into the buffer zBuf[256].  The calling function must
** supply a sufficiently large buffer.
*/
int sqlite3Nebula3RandomSeed(char *zBuf){
  /* We have to initialize zBuf to prevent valgrind from reporting
  ** errors.  The reports issued by valgrind are incorrect - we would
  ** prefer that the randomness be increased by making use of the
  ** uninitialized space in zBuf - but valgrind errors tend to worry
  ** some users.  Rather than argue, it seems easier just to initialize
  ** the whole array and silence valgrind, even if that means less randomness
  ** in the random seed.
  **
  ** When testing, initializing zBuf[] to zero is all we do.  That means
  ** that we always use the same random number sequence.* This makes the
  ** tests repeatable.
  */
  memset(zBuf, 0, 256);
  #if __WII__
  *zBuf = OSGetTime();
  #elif __PS3__

  #else
  GetSystemTime((LPSYSTEMTIME)zBuf);
  #endif
  return SQLITE_OK;
}

/*
** Sleep for a little while.  Return the amount of time slept.
*/
int sqlite3Nebula3Sleep(int ms){
  Core::SysFunc::Sleep(ms / 1000);
  return ms;
}

/*
** Static variables used for thread synchronization
*/
static int inMutex = 0;
static Threading::CriticalSection criticalSection;
#ifdef SQLITE_W32_THREADS
  static DWORD mutexOwner;
  static CRITICAL_SECTION cs;
#endif

/*
** The following pair of routines implement mutual exclusion for
** multi-threaded processes.  Only a single thread is allowed to
** executed code that is surrounded by EnterMutex() and LeaveMutex().
**
** SQLite uses only a single Mutex.  There is not much critical
** code and what little there is executes quickly and without blocking.
**
** Version 3.3.1 and earlier used a simple mutex.  Beginning with
** version 3.3.2, a recursive mutex is required.
*/
void sqlite3Nebula3EnterMutex(){
#ifdef SQLITE_W32_THREADS
  static int isInit = 0;
  while( !isInit ){
    static long lock = 0;
    if( InterlockedIncrement(&lock)==1 ){
      InitializeCriticalSection(&cs);
      isInit = 1;
    }else{
      Sleep(1);
    }
  }
  EnterCriticalSection(&cs);
  mutexOwner = GetCurrentThreadId();
#endif

  criticalSection.Enter();
  inMutex++;
}
void sqlite3Nebula3LeaveMutex(){
  assert( inMutex );
  inMutex--;
  criticalSection.Leave();
#ifdef SQLITE_W32_THREADS
  assert( mutexOwner==GetCurrentThreadId() );
  LeaveCriticalSection(&cs);
#endif
}

/*
** Return TRUE if the mutex is currently held.
**
** If the thisThreadOnly parameter is true, return true if and only if the
** calling thread holds the mutex.  If the parameter is false, return
** true if any thread holds the mutex.
*/
int sqlite3Nebula3InMutex(int thisThreadOnly){
#ifdef SQLITE_W32_THREADS
  return inMutex>0 && (thisThreadOnly==0 || mutexOwner==GetCurrentThreadId());
#else
  return inMutex>0;
#endif
}


/*
** The following variable, if set to a non-zero value, becomes the result
** returned from sqlite3OsCurrentTime().  This is used for testing.
*/
#ifdef SQLITE_TEST
int sqlite3_current_time = 0;
#endif

/*
** Find the current time (in Universal Coordinated Time).  Write the
** current time and date as a Julian Day number into *prNow and
** return 0.  Return 1 if the time and date cannot be found.
*/
int sqlite3Nebula3CurrentTime(double *prNow){
#if __WII__
  *prNow = (double)OSGetTime();
#elif __PS3__
#else
  FILETIME ft;
  /* FILETIME structure is a 64-bit value representing the number of 
     100-nanosecond intervals since January 1, 1601 (= JD 2305813.5). 
  */
  double now;
#if OS_WINCE
  SYSTEMTIME time;
  GetSystemTime(&time);
  SystemTimeToFileTime(&time,&ft);
#else
  GetSystemTimeAsFileTime( &ft );
#endif
  now = ((double)ft.dwHighDateTime) * 4294967296.0; 
  *prNow = (now + ft.dwLowDateTime)/864000000000.0 + 2305813.5;
#ifdef SQLITE_TEST
  if( sqlite3_current_time ){
    *prNow = sqlite3_current_time/86400.0 + 2440587.5;
  }
#endif
#endif
  return 0;
}

/*
** Remember the number of thread-specific-data blocks allocated.
** Use this to verify that we are not leaking thread-specific-data.
** Ticket #1601
*/
#ifdef SQLITE_TEST
int sqlite3_tsd_count = 0;
# define TSD_COUNTER_INCR InterlockedIncrement(&sqlite3_tsd_count)
# define TSD_COUNTER_DECR InterlockedDecrement(&sqlite3_tsd_count)
#else
# define TSD_COUNTER_INCR  /* no-op */
# define TSD_COUNTER_DECR  /* no-op */
#endif


/*
** If called with allocateFlag>1, then return a pointer to thread
** specific data for the current thread.  Allocate and zero the
** thread-specific data if it does not already exist necessary.
**
** If called with allocateFlag==0, then check the current thread
** specific data.  Return it if it exists.  If it does not exist,
** then return NULL.
**
** If called with allocateFlag<0, check to see if the thread specific
** data is allocated and is all zero.  If it is then deallocate it.
** Return a pointer to the thread specific data or NULL if it is
** unallocated or gets deallocated.
*/

ThreadData *sqlite3Nebula3ThreadSpecificData(int allocateFlag){

  ThreadData *pTsd = 0;

#if __PS3__
//static int key = 1;  // index 0 is already used for thread local singletons
//static const ThreadData zeroData = {0};

//pTsd = (ThreadData*) OSGetThreadSpecific(key);

//if( allocateFlag>0 ){
//    if( !pTsd ){
//        pTsd = (ThreadData*)Memory::Alloc(Memory::DefaultHeap, sizeof(zeroData) );
//        if( pTsd ){
//            *pTsd = zeroData;
//            //OSSetThreadSpecific(key, pTsd);
//            TSD_COUNTER_INCR;
//        }
//    }
//}else if( pTsd!=0 && allocateFlag<0 
//         && memcmp(pTsd, &zeroData, sizeof(ThreadData))==0 ){
//             sqlite3OsFree(pTsd);
//             //OSSetThreadSpecific(key, 0);
//             TSD_COUNTER_DECR;
//             pTsd = 0;
//}
#elif __WII__
  static int key = 1;  // index 0 is already used for thread local singletons
  static const ThreadData zeroData = {0};

  pTsd = (ThreadData*) OSGetThreadSpecific(key);

  if( allocateFlag>0 ){
      if( !pTsd ){
          pTsd = (ThreadData*)Memory::Alloc(Memory::DefaultHeap, sizeof(zeroData) );
          if( pTsd ){
              *pTsd = zeroData;
              OSSetThreadSpecific(key, pTsd);
              TSD_COUNTER_INCR;
          }
      }
  }else if( pTsd!=0 && allocateFlag<0 
      && memcmp(pTsd, &zeroData, sizeof(ThreadData))==0 ){
          sqlite3OsFree(pTsd);
          OSSetThreadSpecific(key, 0);
          TSD_COUNTER_DECR;
          pTsd = 0;
  }   
 #else  
  static int key;
  static int keyInit = 0;
  static const ThreadData zeroData = {0};

  if( !keyInit ){
      sqlite3OsEnterMutex();
      if( !keyInit ){
          key = TlsAlloc();
          if( key==0xffffffff ){
              sqlite3OsLeaveMutex();
              return 0;
          }
          keyInit = 1;
      }
      sqlite3OsLeaveMutex();
  }
  pTsd = (ThreadData*)TlsGetValue(key);
  if( allocateFlag>0 ){
      if( !pTsd ){
          pTsd = (ThreadData*)Memory::Alloc(Memory::DefaultHeap, sizeof(zeroData) );
          if( pTsd ){
              *pTsd = zeroData;
              TlsSetValue(key, pTsd);
              TSD_COUNTER_INCR;
          }
      }
  }else if( pTsd!=0 && allocateFlag<0 
      && memcmp(pTsd, &zeroData, sizeof(ThreadData))==0 ){
          sqlite3OsFree(pTsd);
          TlsSetValue(key, 0);
          TSD_COUNTER_DECR;
          pTsd = 0;
  }
 #endif
  return pTsd;
}


#if !defined(SQLITE_OMIT_LOAD_EXTENSION)
/*
** Interfaces for opening a shared library, finding entry points
** within the shared library, and closing the shared library.
*/
void *sqlite3Nebula3Dlopen(const char *zFilename){
    return 0;
}
void *sqlite3Nebula3Dlsym(void *pHandle, const char *zSymbol){
    return 0;
}
int sqlite3Nebula3Dlclose(void *pHandle){
  return 0;
}

#endif /* !SQLITE_OMIT_LOAD_EXTENSION */

}