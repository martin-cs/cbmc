// _Atomic is a C11 keyword. It can be used as a type qualifier
// and as a type specifier, which introduces ambiguity into the grammar.
               
// 6.7.2.4 - 4: If the _Atomic keyword is immediately followed by a left
// parenthesis, it is interpreted as a type specifier (with a type name),
// not as a type qualifier. 

// Visual Studio doesn't have it, will likely never have it.

#ifdef __GNUC__
_Atomic(int) x1; // type specifier
_Atomic volatile int x2; // type qualifier
#endif

int main()
{
}
