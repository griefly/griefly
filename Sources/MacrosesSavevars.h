#pragma once



#define CONTAC(A, B)\
        A##B

#define SAVEBLE(Type, Name)                                                  \
        Type Name;                                                           \
        __forceinline void CONTAC(__KV_FUNC_S_, Name)(std::strstream& file)  \
        {                                                                    \
            __KV_BEF_S_FUNC;                                                 \
            file << N1;                                                      \
        };                                                                   \
        __forceinline void CONTAC(__KV_FUNC_L_, Name)(std::strstream& file)  \
        {                                                                    \                
            __KV_BEF_L_FUNC;                                                 \
            file >> N1;                                                      \
        };
