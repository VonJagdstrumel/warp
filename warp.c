#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>
#include <direct.h>
#include <process.h>

#define LIST_RES_ID 4200
#define BASE_RES_ID 9000
#define TMP_DIR_SUFFIX "warp\\"

typedef struct resource {
  HGLOBAL hMemory;
  DWORD dwSize;
  LPVOID lpAddress;
} resource_t;

typedef struct list {
  void** pContent;
  size_t size;
} list_t;

void* cmalloc(size_t size) {
  void* pBuffer = malloc(size);

  if(pBuffer == NULL) {
    exit(1);
  }

  return pBuffer;
}

void* crealloc(void* ptr, size_t size) {
  void* pBuffer = realloc(ptr, size);

  if(pBuffer == NULL) {
    exit(1);
  }

  return pBuffer;
}

list_t* list_create() {
  list_t* pList = (list_t*) cmalloc(sizeof(list_t));
  pList->pContent = (void**) cmalloc(sizeof(void*) * 10);
  pList->size = 0;

  return pList;
}

void list_add(list_t* pList, void* pItem) {
  if(pList->size > 0 && pList->size % 10 == 0) {
    pList->pContent = (void**) crealloc(pList->pContent, pList->size + 10);
  }

  pList->pContent[pList->size] = pItem;
  ++pList->size;
}

void list_destroy(list_t* pList) {
  if(pList->size > 0) {
    free(pList->pContent[0]);
  }

  free(pList->pContent);
  free(pList);
  pList = NULL;
}

resource_t* resource_create(HMODULE hModule, WORD wResourceId) {
  resource_t* pResource = (resource_t*) cmalloc(sizeof(resource_t));

  HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(wResourceId), RT_RCDATA);
  pResource->hMemory = LoadResource(hModule, hResource);
  pResource->dwSize = SizeofResource(hModule, hResource);
  pResource->lpAddress = LockResource(pResource->hMemory);

  return pResource;
}

void resource_destroy(resource_t* pResource) {
  FreeResource(pResource->hMemory);
  free(pResource);
  pResource = NULL;
}

list_t* list_parse(resource_t* pResource) {
  char* pBuffer = (char*) cmalloc(pResource->dwSize + 1);
  memcpy(pBuffer, pResource->lpAddress, pResource->dwSize);
  pBuffer[pResource->dwSize] = '\0';

  list_t* pList = list_create();
  char* pLine = strtok(pBuffer, "\n");
  while(pLine != NULL && strlen(pLine) > 0) {
    list_add(pList, pLine);
    pLine = strtok(NULL, "\n");
  }

  return pList;
}

char* dstpath_create() {
  char* pTmpPath = getenv("TMP");
  char* pTmpDirPrefix = tmpnam(NULL);
  char* pDstPath = (char*) cmalloc(strlen(pTmpPath) + strlen(pTmpDirPrefix) + strlen(TMP_DIR_SUFFIX) + 1);

  sprintf(pDstPath, "%s%s%s", pTmpPath, pTmpDirPrefix, TMP_DIR_SUFFIX);
  CreateDirectory(pDstPath, NULL);

  return pDstPath;
}

char* fullpath_create(char* pDstPath, char* pSubPath) {
  char* pFullPath = (char*) cmalloc(strlen(pDstPath) + strlen(pSubPath) + 1);
  char* pChar = strrchr(pSubPath, '/');

  strcpy(pFullPath, pDstPath);
  if(pChar == NULL) {
    strcat(pFullPath, pSubPath);
  }
  else {
    strncat(pFullPath, pSubPath, pChar - pSubPath);
    CreateDirectory(pFullPath, NULL);
    strcat(pFullPath, pChar);
  }

  return pFullPath;
}

void resource_unpack(resource_t* pResource, char* pFullPath) {
  FILE* pFile = fopen(pFullPath, "wb");

  if(pFile == NULL) {
    exit(1);
  }

  fwrite(pResource->lpAddress, 1, pResource->dwSize, pFile);
  fclose(pFile);
}

char* execname_get(char* pFullPath) {
  char* pChar = strrchr(pFullPath, '\\');
  char* pExecPath;

  if(pChar == NULL) {
    pChar = pFullPath;
  }
  else {
    pChar = pChar + 1;
  }

  pExecPath = (char*) cmalloc(strlen(pChar) + 1);
  strcpy(pExecPath, pChar);

  return pExecPath;
}

int main(int argc, char** argv) {
  HMODULE hModule = GetModuleHandle(NULL);
  resource_t* pResource = resource_create(hModule, LIST_RES_ID);
  list_t* pList = list_parse(pResource);
  resource_destroy(pResource);

  char* pFullPath;
  char* pDstPath = dstpath_create();
  char* pExecPath = NULL;

  for(int i = 0; i < pList->size; ++i) {
    pResource = resource_create(hModule, BASE_RES_ID + i);
    pFullPath = fullpath_create(pDstPath, (char*) pList->pContent[i]);

    resource_unpack(pResource, pFullPath);

    if(i == 0) {
      pExecPath = execname_get(pFullPath);
    }

    free(pFullPath);
    resource_destroy(pResource);
  }

  list_destroy(pList);

  if(pList->size > 0) {
    _chdir(pDstPath);
    _execv(pExecPath, (const char* const*) argv);

    // TODO: Must not return to parent process
  }

  return 1;
}
