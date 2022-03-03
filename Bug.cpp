
class A
{
public:
  virtual void afunc () {};
};

class B
{
public:
  virtual ~B() {}; // comment out to cause crash
  virtual void bfunc () {};
};

class C : public A, public B
{
};

int main (void)
{
  C * Cptr = new C();
  B * Bptr = Cptr;

#if 0
  // Bptr--;  // makes it work

  // prove that Bptr != Cptr
  if (*((char *)(&Bptr)) != *((char *)(&Cptr)))
    *((char *)0) = 'x'; // crash
#endif

  delete Bptr;

  return 0;
}
