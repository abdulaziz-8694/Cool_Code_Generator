
//**************************************************************
//
// Code generator SKELETON
//
// Read the comments carefully. Make sure to
//    initialize the base class tags in
//       `CgenClassTable::CgenClassTable'
//
//    Add the label for the dispatch tables to
//       `IntEntry::code_def'
//       `StringEntry::code_def'
//       `BoolConst::code_def'
//
//    Add code to emit everyting else that is needed
//       in `CgenClassTable::code'
//
//
// The files as provided will produce code to begin the code
// segments, declare globals, and emit constants.  You must
// fill in the rest.
//
//**************************************************************

#include "cgen.h"
#include "cgen_gc.h"
#include <map>
#include <vector>

extern void emit_string_constant(ostream& str, char *s);
extern int cgen_debug;

//
// Three symbols from the semantic analyzer (semant.cc) are used.
// If e : No_type, then no code is generated for e.
// Special code is generated for new SELF_TYPE.
// The name "self" also generates code different from other references.
//
//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
Symbol 
       arg,
       arg2,
       Bool,
       concat,
       cool_abort,
       copy,
       Int,
       in_int,
       in_string,
       IO,
       length,
       Main,
       main_meth,
       No_class,
       No_type,
       Object,
       out_int,
       out_string,
       prim_slot,
       self,
       SELF_TYPE,
       Str,
       str_field,
       substr,
       type_name,
       val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
  arg         = idtable.add_string("arg");
  arg2        = idtable.add_string("arg2");
  Bool        = idtable.add_string("Bool");
  concat      = idtable.add_string("concat");
  cool_abort  = idtable.add_string("abort");
  copy        = idtable.add_string("copy");
  Int         = idtable.add_string("Int");
  in_int      = idtable.add_string("in_int");
  in_string   = idtable.add_string("in_string");
  IO          = idtable.add_string("IO");
  length      = idtable.add_string("length");
  Main        = idtable.add_string("Main");
  main_meth   = idtable.add_string("main");
//   _no_class is a symbol that can't be the name of any 
//   user-defined class.
  No_class    = idtable.add_string("_no_class");
  No_type     = idtable.add_string("_no_type");
  Object      = idtable.add_string("Object");
  out_int     = idtable.add_string("out_int");
  out_string  = idtable.add_string("out_string");
  prim_slot   = idtable.add_string("_prim_slot");
  self        = idtable.add_string("self");
  SELF_TYPE   = idtable.add_string("SELF_TYPE");
  Str         = idtable.add_string("String");
  str_field   = idtable.add_string("_str_field");
  substr      = idtable.add_string("substr");
  type_name   = idtable.add_string("type_name");
  val         = idtable.add_string("_val");
}

static char *gc_init_names[] =
  { "_NoGC_Init", "_GenGC_Init", "_ScnGC_Init" };
static char *gc_collect_names[] =
  { "_NoGC_Collect", "_GenGC_Collect", "_ScnGC_Collect" };


//  BoolConst is a class that implements code generation for operations
//  on the two booleans, which are given global names here.
BoolConst falsebool(FALSE);
BoolConst truebool(TRUE);

//*********************************************************
//
// Define method for code generation
//
// This is the method called by the compiler driver
// `cgtest.cc'. cgen takes an `ostream' to which the assembly will be
// emmitted, and it passes this and the class list of the
// code generator tree to the constructor for `CgenClassTable'.
// That constructor performs all of the work of the code
// generator.
//
//*********************************************************

/*Modified Code Starts Here
  Method Table and attribute Table to get the offset and name of feature.
*/
std::map<Symbol,std::map<Symbol,std::pair<Symbol,int> > > dispatchTable;    //Dispatch table for every class
std::map<Symbol,std::map<Symbol,std::pair<Symbol,int> > > attrTable;        //Attribue Table for every class
Symbol cur_classname;                                                       //Current classname
unsigned label_count = 0;                                                   // count of the label for branching
std::vector<Symbol> letScope;                                               // For Handling Let Scopes
std::vector<Symbol> formalScope;                                            // For Handling Formal Arguments Scope
/*Modified Code Ends Here*/

void program_class::cgen(ostream &os) 
{
  // spim wants comments to start with '#'
  os << "# start of generated code\n";

  initialize_constants();
  CgenClassTable *codegen_classtable = new CgenClassTable(classes,os);

  os << "\n# end of generated code\n";
}


//////////////////////////////////////////////////////////////////////////////
//
//  emit_* procedures
//
//  emit_X  writes code for operation "X" to the output stream.
//  There is an emit_X for each opcode X, as well as emit_ functions
//  for generating names according to the naming conventions (see emit.h)
//  and calls to support functions defined in the trap handler.
//
//  Register names and addresses are passed as strings.  See `emit.h'
//  for symbolic names you can use to refer to the strings.
//
//////////////////////////////////////////////////////////////////////////////

static void emit_load(char *dest_reg, int offset, char *source_reg, ostream& s)
{
  s << LW << dest_reg << " " << offset * WORD_SIZE << "(" << source_reg << ")" 
    << endl;
}

static void emit_store(char *source_reg, int offset, char *dest_reg, ostream& s)
{
  s << SW << source_reg << " " << offset * WORD_SIZE << "(" << dest_reg << ")"
      << endl;
}

static void emit_load_imm(char *dest_reg, int val, ostream& s)
{ s << LI << dest_reg << " " << val << endl; }

static void emit_load_address(char *dest_reg, char *address, ostream& s)
{ s << LA << dest_reg << " " << address << endl; }

static void emit_partial_load_address(char *dest_reg, ostream& s)
{ s << LA << dest_reg << " "; }

static void emit_load_bool(char *dest, const BoolConst& b, ostream& s)
{
  emit_partial_load_address(dest,s);
  b.code_ref(s);
  s << endl;
}

static void emit_load_string(char *dest, StringEntry *str, ostream& s)
{
  emit_partial_load_address(dest,s);
  str->code_ref(s);
  s << endl;
}

static void emit_load_int(char *dest, IntEntry *i, ostream& s)
{
  emit_partial_load_address(dest,s);
  i->code_ref(s);
  s << endl;
}

static void emit_move(char *dest_reg, char *source_reg, ostream& s)
{ s << MOVE << dest_reg << " " << source_reg << endl; }

static void emit_neg(char *dest, char *src1, ostream& s)
{ s << NEG << dest << " " << src1 << endl; }

static void emit_add(char *dest, char *src1, char *src2, ostream& s)
{ s << ADD << dest << " " << src1 << " " << src2 << endl; }

static void emit_addu(char *dest, char *src1, char *src2, ostream& s)
{ s << ADDU << dest << " " << src1 << " " << src2 << endl; }

static void emit_addiu(char *dest, char *src1, int imm, ostream& s)
{ s << ADDIU << dest << " " << src1 << " " << imm << endl; }

static void emit_div(char *dest, char *src1, char *src2, ostream& s)
{ s << DIV << dest << " " << src1 << " " << src2 << endl; }

static void emit_mul(char *dest, char *src1, char *src2, ostream& s)
{ s << MUL << dest << " " << src1 << " " << src2 << endl; }

static void emit_sub(char *dest, char *src1, char *src2, ostream& s)
{ s << SUB << dest << " " << src1 << " " << src2 << endl; }

static void emit_sll(char *dest, char *src1, int num, ostream& s)
{ s << SLL << dest << " " << src1 << " " << num << endl; }

static void emit_jalr(char *dest, ostream& s)
{ s << JALR << "\t" << dest << endl; }

static void emit_jal(char *address,ostream &s)
{ s << JAL << address << endl; }

static void emit_return(ostream& s)
{ s << RET << endl; }

static void emit_gc_assign(ostream& s)
{ s << JAL << "_GenGC_Assign" << endl; }

static void emit_disptable_ref(Symbol sym, ostream& s)
{  s << sym << DISPTAB_SUFFIX; }

static void emit_init_ref(Symbol sym, ostream& s)
{ s << sym << CLASSINIT_SUFFIX; }

static void emit_label_ref(int l, ostream &s)
{ s << "label" << l; }

static void emit_protobj_ref(Symbol sym, ostream& s)
{ s << sym << PROTOBJ_SUFFIX; }

static void emit_method_ref(Symbol classname, Symbol methodname, ostream& s)
{ s << classname << METHOD_SEP << methodname; }

static void emit_label_def(int l, ostream &s)
{
  emit_label_ref(l,s);
  s << ":" << endl;
}

static void emit_beqz(char *source, int label, ostream &s)
{
  s << BEQZ << source << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_beq(char *src1, char *src2, int label, ostream &s)
{
  s << BEQ << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_bne(char *src1, char *src2, int label, ostream &s)
{
  s << BNE << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_bleq(char *src1, char *src2, int label, ostream &s)
{
  s << BLEQ << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_blt(char *src1, char *src2, int label, ostream &s)
{
  s << BLT << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_blti(char *src1, int imm, int label, ostream &s)
{
  s << BLT << src1 << " " << imm << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_bgti(char *src1, int imm, int label, ostream &s)
{
  s << BGT << src1 << " " << imm << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_branch(int l, ostream& s)
{
  s << BRANCH;
  emit_label_ref(l,s);
  s << endl;
}

//
// Push a register on the stack. The stack grows towards smaller addresses.
//
static void emit_push(char *reg, ostream& str)
{
  emit_store(reg,0,SP,str);
  emit_addiu(SP,SP,-4,str);
}

//
// Fetch the integer value in an Int object.
// Emits code to fetch the integer value of the Integer object pointed
// to by register source into the register dest
//
static void emit_fetch_int(char *dest, char *source, ostream& s)
{ emit_load(dest, DEFAULT_OBJFIELDS, source, s); }

//
// Emits code to store the integer value contained in register source
// into the Integer object pointed to by dest.
//
static void emit_store_int(char *source, char *dest, ostream& s)
{ emit_store(source, DEFAULT_OBJFIELDS, dest, s); }


static void emit_test_collector(ostream &s)
{
  emit_push(ACC, s);
  emit_move(ACC, SP, s); // stack end
  emit_move(A1, ZERO, s); // allocate nothing
  s << JAL << gc_collect_names[cgen_Memmgr] << endl;
  emit_addiu(SP,SP,4,s);
  emit_load(ACC,0,SP,s);
}

static void emit_gc_check(char *source, ostream &s)
{
  if (source != (char*)A1) emit_move(A1, source, s);
  s << JAL << "_gc_check" << endl;
}

/*Pop the top of stack and store it the destination register*/
static void emit_pop(char *dest, ostream &s)
{
  emit_load(dest,1,SP,s);
  emit_addiu(SP,SP,4,s);
}


///////////////////////////////////////////////////////////////////////////////
//
// coding strings, ints, and booleans
//
// Cool has three kinds of constants: strings, ints, and booleans.
// This section defines code generation for each type.
//
// All string constants are listed in the global "stringtable" and have
// type StringEntry.  StringEntry methods are defined both for String
// constant definitions and references.
//
// All integer constants are listed in the global "inttable" and have
// type IntEntry.  IntEntry methods are defined for Int
// constant definitions and references.
//
// Since there are only two Bool values, there is no need for a table.
// The two booleans are represented by instances of the class BoolConst,
// which defines the definition and reference methods for Bools.
//
///////////////////////////////////////////////////////////////////////////////

//
// Strings
//
void StringEntry::code_ref(ostream& s)
{
  s << STRCONST_PREFIX << index;
}

//
// Emit code for a constant String.
// You should fill in the code naming the dispatch table.
//

void StringEntry::code_def(ostream& s, int stringclasstag)
{
  IntEntryP lensym = inttable.add_int(len);

  // Add -1 eye catcher
  s << WORD << "-1" << endl;

  code_ref(s);  s  << LABEL                                             // label
      << WORD << stringclasstag << endl                                 // tag
      << WORD << (DEFAULT_OBJFIELDS + STRING_SLOTS + (len+4)/4) << endl // size
      << WORD;


 /***** Add dispatch information for class String ******/
      /*Modified Code Starts*/
      s << STRINGNAME<<DISPTAB_SUFFIX;
      /*Modified Code Ends*/ 
      s << endl;                                              // dispatch table
      s << WORD;  lensym->code_ref(s);  s << endl;            // string length
  emit_string_constant(s,str);                                // ascii string
  s << ALIGN;                                                 // align to word
}

//
// StrTable::code_string
// Generate a string object definition for every string constant in the 
// stringtable.
//
void StrTable::code_string_table(ostream& s, int stringclasstag)
{  
  for (List<StringEntry> *l = tbl; l; l = l->tl())
    l->hd()->code_def(s,stringclasstag);
}

//
// Ints
//
void IntEntry::code_ref(ostream &s)
{
  s << INTCONST_PREFIX << index;
}

//
// Emit code for a constant Integer.
// You should fill in the code naming the dispatch table.
//

void IntEntry::code_def(ostream &s, int intclasstag)
{
  // Add -1 eye catcher
  s << WORD << "-1" << endl;

  code_ref(s);  s << LABEL                                // label
      << WORD << intclasstag << endl                      // class tag
      << WORD << (DEFAULT_OBJFIELDS + INT_SLOTS) << endl  // object size
      << WORD; 

 /***** Add dispatch information for class Int ******/
      /*Modified Code Starts*/
      s << INTNAME<<DISPTAB_SUFFIX;
      /*Modified Code Ends*/
      s << endl;                                          // dispatch table
      s << WORD << str << endl;                           // integer value
}


//
// IntTable::code_string_table
// Generate an Int object definition for every Int constant in the
// inttable.
//
void IntTable::code_string_table(ostream &s, int intclasstag)
{
  for (List<IntEntry> *l = tbl; l; l = l->tl())
    l->hd()->code_def(s,intclasstag);
}


//
// Bools
//
BoolConst::BoolConst(int i) : val(i) { assert(i == 0 || i == 1); }

void BoolConst::code_ref(ostream& s) const
{
  s << BOOLCONST_PREFIX << val;
}
  
//
// Emit code for a constant Bool.
// You should fill in the code naming the dispatch table.
//

void BoolConst::code_def(ostream& s, int boolclasstag)
{
  // Add -1 eye catcher
  s << WORD << "-1" << endl;

  code_ref(s);  s << LABEL                                  // label
      << WORD << boolclasstag << endl                       // class tag
      << WORD << (DEFAULT_OBJFIELDS + BOOL_SLOTS) << endl   // object size
      << WORD;

 /***** Add dispatch information for class Bool ******/
      /*Modified Code Starts*/
      s << BOOLNAME<<DISPTAB_SUFFIX;
      /*Modified Code Ends*/
      s << endl;                                            // dispatch table
      s << WORD << val << endl;                             // value (0 or 1)
}

//////////////////////////////////////////////////////////////////////////////
//
//  CgenClassTable methods
//
//////////////////////////////////////////////////////////////////////////////

//***************************************************
//
//  Emit code to start the .data segment and to
//  declare the global names.
//
//***************************************************

void CgenClassTable::code_global_data()
{
  Symbol main    = idtable.lookup_string(MAINNAME);
  Symbol string  = idtable.lookup_string(STRINGNAME);
  Symbol integer = idtable.lookup_string(INTNAME);
  Symbol boolc   = idtable.lookup_string(BOOLNAME);

  str << "\t.data\n" << ALIGN;
  //
  // The following global names must be defined first.
  //
  str << GLOBAL << CLASSNAMETAB << endl;
  str << GLOBAL; emit_protobj_ref(main,str);    str << endl;
  str << GLOBAL; emit_protobj_ref(integer,str); str << endl;
  str << GLOBAL; emit_protobj_ref(string,str);  str << endl;
  str << GLOBAL; falsebool.code_ref(str);  str << endl;
  str << GLOBAL; truebool.code_ref(str);   str << endl;
  str << GLOBAL << INTTAG << endl;
  str << GLOBAL << BOOLTAG << endl;
  str << GLOBAL << STRINGTAG << endl;

  //
  // We also need to know the tag of the Int, String, and Bool classes
  // during code generation.
  //
  str << INTTAG << LABEL
      << WORD << intclasstag << endl;
  str << BOOLTAG << LABEL 
      << WORD << boolclasstag << endl;
  str << STRINGTAG << LABEL 
      << WORD << stringclasstag << endl;    
}


//***************************************************
//
//  Emit code to start the .text segment and to
//  declare the global names.
//
//***************************************************

void CgenClassTable::code_global_text()
{
  str << GLOBAL << HEAP_START << endl
      << HEAP_START << LABEL 
      << WORD << 0 << endl
      << "\t.text" << endl
      << GLOBAL;
  emit_init_ref(idtable.add_string("Main"), str);
  str << endl << GLOBAL;
  emit_init_ref(idtable.add_string("Int"),str);
  str << endl << GLOBAL;
  emit_init_ref(idtable.add_string("String"),str);
  str << endl << GLOBAL;
  emit_init_ref(idtable.add_string("Bool"),str);
  str << endl << GLOBAL;
  emit_method_ref(idtable.add_string("Main"), idtable.add_string("main"), str);
  str << endl;
}

void CgenClassTable::code_bools(int boolclasstag)
{
  falsebool.code_def(str,boolclasstag);
  truebool.code_def(str,boolclasstag);
}

void CgenClassTable::code_select_gc()
{
  //
  // Generate GC choice constants (pointers to GC functions)
  //
  str << GLOBAL << "_MemMgr_INITIALIZER" << endl;
  str << "_MemMgr_INITIALIZER:" << endl;
  str << WORD << gc_init_names[cgen_Memmgr] << endl;
  str << GLOBAL << "_MemMgr_COLLECTOR" << endl;
  str << "_MemMgr_COLLECTOR:" << endl;
  str << WORD << gc_collect_names[cgen_Memmgr] << endl;
  str << GLOBAL << "_MemMgr_TEST" << endl;
  str << "_MemMgr_TEST:" << endl;
  str << WORD << (cgen_Memmgr_Test == GC_TEST) << endl;
}


//********************************************************
//
// Emit code to reserve space for and initialize all of
// the constants.  Class names should have been added to
// the string table (in the supplied code, is is done
// during the construction of the inheritance graph), and
// code for emitting string constants as a side effect adds
// the string's length to the integer table.  The constants
// are emmitted by running through the stringtable and inttable
// and producing code for each entry.
//
//********************************************************

void CgenClassTable::code_constants()
{
  //
  // Add constants that are required by the code generator.
  //
  stringtable.add_string("");
  inttable.add_string("0");

  stringtable.code_string_table(str,stringclasstag);
  inttable.code_string_table(str,intclasstag);
  code_bools(boolclasstag);
}


CgenClassTable::CgenClassTable(Classes classes, ostream& s) : nds(NULL) , str(s)
{
   stringclasstag = 4 /* Change to your String class tag here */;
   intclasstag =    2 /* Change to your Int class tag here */;
   boolclasstag =   3 /* Change to your Bool class tag here */;

   enterscope();
   if (cgen_debug) cout << "Building CgenClassTable" << endl;
   install_basic_classes();
   install_classes(classes);
   build_inheritance_tree();
   for (List<CgenNode> *l = nds; l; l = l->tl())
   {
    ordered_class.push_back(l->hd());
   }
   std::reverse(ordered_class.begin(),ordered_class.end());
   code();
   exitscope();
}

void CgenClassTable::install_basic_classes()
{

// The tree package uses these globals to annotate the classes built below.
  //curr_lineno  = 0;
  Symbol filename = stringtable.add_string("<basic class>");

//
// A few special class names are installed in the lookup table but not
// the class list.  Thus, these classes exist, but are not part of the
// inheritance hierarchy.
// No_class serves as the parent of Object and the other special classes.
// SELF_TYPE is the self class; it cannot be redefined or inherited.
// prim_slot is a class known to the code generator.
//
  addid(No_class,
  new CgenNode(class_(No_class,No_class,nil_Features(),filename),
          Basic,this));
  addid(SELF_TYPE,
  new CgenNode(class_(SELF_TYPE,No_class,nil_Features(),filename),
          Basic,this));
  addid(prim_slot,
  new CgenNode(class_(prim_slot,No_class,nil_Features(),filename),
          Basic,this));

// 
// The Object class has no parent class. Its methods are
//        cool_abort() : Object    aborts the program
//        type_name() : Str        returns a string representation of class name
//        copy() : SELF_TYPE       returns a copy of the object
//
// There is no need for method bodies in the basic classes---these
// are already built in to the runtime system.
//
  install_class(
   new CgenNode(
    class_(Object, 
     No_class,
     append_Features(
           append_Features(
           single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
           single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
           single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
     filename),
    Basic,this));

// 
// The IO class inherits from Object. Its methods are
//        out_string(Str) : SELF_TYPE          writes a string to the output
//        out_int(Int) : SELF_TYPE               "    an int    "  "     "
//        in_string() : Str                    reads a string from the input
//        in_int() : Int                         "   an int     "  "     "
//
   install_class(
    new CgenNode(
     class_(IO, 
            Object,
            append_Features(
            append_Features(
            append_Features(
            single_Features(method(out_string, single_Formals(formal(arg, Str)),
                        SELF_TYPE, no_expr())),
            single_Features(method(out_int, single_Formals(formal(arg, Int)),
                        SELF_TYPE, no_expr()))),
            single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
            single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
     filename),     
    Basic,this));

//
// The Int class has no methods and only a single attribute, the
// "val" for the integer. 
//
   install_class(
    new CgenNode(
     class_(Int, 
      Object,
            single_Features(attr(val, prim_slot, no_expr())),
      filename),
     Basic,this));

//
// Bool also has only the "val" slot.
//
    install_class(
     new CgenNode(
      class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename),
      Basic,this));

//
// The class Str has a number of slots and operations:
//       val                                  ???
//       str_field                            the string itself
//       length() : Int                       length of the string
//       concat(arg: Str) : Str               string concatenation
//       substr(arg: Int, arg2: Int): Str     substring
//       
   install_class(
    new CgenNode(
      class_(Str, 
       Object,
             append_Features(
             append_Features(
             append_Features(
             append_Features(
             single_Features(attr(val, Int, no_expr())),
            single_Features(attr(str_field, prim_slot, no_expr()))),
            single_Features(method(length, nil_Formals(), Int, no_expr()))),
            single_Features(method(concat, 
           single_Formals(formal(arg, Str)),
           Str, 
           no_expr()))),
      single_Features(method(substr, 
           append_Formals(single_Formals(formal(arg, Int)), 
              single_Formals(formal(arg2, Int))),
           Str, 
           no_expr()))),
       filename),
        Basic,this));

}

// CgenClassTable::install_class
// CgenClassTable::install_classes
//
// install_classes enters a list of classes in the symbol table.
//
void CgenClassTable::install_class(CgenNodeP nd)
{
  Symbol name = nd->get_name();

  if (probe(name))
    {
      return;
    }

  // The class name is legal, so add it to the list of classes
  // and the symbol table.
  nds = new List<CgenNode>(nd,nds);
  addid(name,nd);
}

void CgenClassTable::install_classes(Classes cs)
{
  for(int i = cs->first(); cs->more(i); i = cs->next(i))
    install_class(new CgenNode(cs->nth(i),NotBasic,this));
}

//
// CgenClassTable::build_inheritance_tree
//
void CgenClassTable::build_inheritance_tree()
{
  for(List<CgenNode> *l = nds; l; l = l->tl())
      set_relations(l->hd());
}

//
// CgenClassTable::set_relations
//
// Takes a CgenNode and locates its, and its parent's, inheritance nodes
// via the class table.  Parent and child pointers are added as appropriate.
//
void CgenClassTable::set_relations(CgenNodeP nd)
{
  CgenNode *parent_node = probe(nd->get_parent());
  nd->set_parentnd(parent_node);
  parent_node->add_child(nd);
}

void CgenNode::add_child(CgenNodeP n)
{
  children = new List<CgenNode>(n,children);
}

void CgenNode::set_parentnd(CgenNodeP p)
{
  assert(parentnd == NULL);
  assert(p != NULL);
  parentnd = p;
}
/**********************************************
 * Modified Code Starts here                  *
* Emitting class Name Table, Object Tables,   *
* Dispatch Tables for each classes and also   *
* prototype object table for each class       *
***********************************************/

//Coding class_nameTab for every class. Contains the name of each class
void CgenClassTable::code_class_nameTab()
{
  str << CLASSNAMETAB << LABEL;
  for (unsigned i = 0;i<ordered_class.size();i++)
  {
    char *stringname = (ordered_class[i])->get_name()->get_string();;
    StringEntry *string = stringtable.lookup_string(stringname);
    str<<WORD;
    string->code_ref(str);
    str<<endl;
  }

}

//Coding Object table for each class. Contains label for prototype Objects and Init methods
void CgenClassTable::code_class_obj()
{
  str << CLASSOBJTAB << LABEL;
  for (unsigned i = 0;i<ordered_class.size();i++)
  {
    str << WORD << (ordered_class[i])->get_name() << PROTOBJ_SUFFIX << endl;
    str << WORD << (ordered_class[i])->get_name() << CLASSINIT_SUFFIX << endl; 
  }
}


/*Populating Dispatch Table Starting from ancestors to successors*/
void CgenClassTable::populate_dispachTable(CgenNodeP node, CgenNodeP cur_class)
{
  //First populate methods from the parent then itself
  if(node->get_name()!=Object)
  {
    populate_dispachTable(node->get_parentnd(),cur_class);
  }
  

  Features f = node->features;
  for(int i = f->first();f->more(i);i = f->next(i))
  {
    /*Verifying whether the feature is a method or not*/
    if(method_class * method = dynamic_cast<method_class *>(f->nth(i)))
    {
      /*If not defined in the parent class*/
      if((dispatchTable.find(cur_class->get_name())->second).find(method->name) == (dispatchTable.find(cur_class->get_name())->second).end())
      {
        int offset = dispatchTable.find(cur_class->get_name())->second.size();
        dispatchTable.find(cur_class->get_name())->second.insert(std::pair<Symbol,std::pair<Symbol,int> >(method->name,std::pair<Symbol,int>(node->get_name(),offset)));
      }
      else  //Overriding methods in the current class
      {
        ((dispatchTable.find(cur_class->get_name())->second).find(method->name)->second).first = node->get_name();
      }
    }
  }
}

//Compare function for two pairs based on the offset
bool compare(std::pair<Symbol,int> a, std::pair<Symbol,int> b)
{
  return a.second<b.second;
}

/*
emitting code for the dispatch Table for each class
*/
void CgenClassTable::emit_dispatchTable()
{
  for(unsigned i=0;i<ordered_class.size();i++)
  {
    //Emitting Dispatch Table for each class
    str<< ordered_class[i]->get_name()<< "_dispTab"<<LABEL;

    //Taking the method list for the present class
    std::map< Symbol,std::pair<Symbol,int> > method_list = dispatchTable.find(ordered_class[i]->get_name())->second;
    std::vector< std::pair<Symbol,int> > ordered_dispatch;  //Vector for sorting in the order of offsets.
    
    for(std::map<Symbol,std::pair<Symbol,int> >::iterator it = method_list.begin();it!=method_list.end();it++)
    {
      ordered_dispatch.push_back(std::pair<Symbol,int>(it->first,(it->second).second));
    }

    /*For emmitting in the order of their offsets in the map*/
    std::sort(ordered_dispatch.begin(),ordered_dispatch.end(),compare);
    for(unsigned i = 0; i<ordered_dispatch.size();i++)
    {
      str<< WORD << method_list.find(ordered_dispatch[i].first)->second.first <<"."<< ordered_dispatch[i].first<<endl;

    }
  }
}
/*Code Dispatch Table for each class*/
void CgenClassTable::code_dispatchTable()
{
  for(unsigned i=0;i<ordered_class.size();i++)
  {
    //starting with an empty method list
    std::map<Symbol,std::pair<Symbol, int> > method_list;
    dispatchTable.insert(std::pair<Symbol, std::map<Symbol,std::pair<Symbol,int> > >(ordered_class[i]->get_name(),method_list));
    populate_dispachTable(ordered_class[i],ordered_class[i]);
  }

  //emmiting dispatch Tables.
  emit_dispatchTable();
}
/*Populating attributes for each class starting from ancestor to successor*/
void CgenClassTable::populate_attrTable(CgenNodeP node, CgenNodeP cur_class)
{
  //First populate attributes of the parent class
  if(node->get_name()!=Object)
  {
    populate_attrTable(node->get_parentnd(),cur_class);
  }

  Features f = node->features;
  for(int i = f->first();f->more(i); i=f->next(i))
  {
    /*Verifying if the feature is an attribute*/
    attr_class *attr = dynamic_cast<attr_class*>(f->nth(i));
    if(attr!=NULL)
    {
      int offset = (attrTable.find(cur_class->get_name())->second).size();
      (attrTable.find(cur_class->get_name())->second).insert(std::pair<Symbol,std::pair<Symbol,int> >(attr->name,std::pair<Symbol,int>(attr->type_decl,offset)));
    }
  }
}


/*Emmitting code for a particular class*/
void CgenClassTable::emit_attributes(CgenNodeP cur_class)
{
  std::map<Symbol, std::pair<Symbol,int> >attr_list = attrTable.find(cur_class->get_name())->second;
  std::vector<std::pair<Symbol,int> > ordered_attr;
  for(std::map<Symbol,std::pair<Symbol,int> >::iterator it = attr_list.begin();it!=attr_list.end();it++)
  {
    ordered_attr.push_back(std::pair<Symbol,int>(it->first,(it->second).second));
  }
  /*For printing the attributes in the order of their offsets in the table*/
  std::sort(ordered_attr.begin(),ordered_attr.end(),compare);
  for(unsigned i = 0; i<ordered_attr.size();i++)
  {
    str<<WORD;
    Symbol type = (attr_list.find(ordered_attr[i].first)->second).first;
    //if(cgen_debug) cout<<cur_class->get_name()<<"\t"<<type<<endl;
    if(type == Int){
      IntEntry *e = inttable.lookup_string("0");
      e->code_ref(str);
      str<<endl;
    }
    else if(type == Str){
      StringEntry *e = stringtable.lookup_string("");
      e->code_ref(str);
      str<<endl;
    }
    else if(type == Bool){
      falsebool.code_ref(str);
      str<<endl;
    }
    else
      str<<0<<endl;
  }
}

/*Coding the prototype Objects for each class*/
void CgenClassTable::code_class_protObj()
{
  for(unsigned i = 0; i<ordered_class.size();i++)
  {
    std::map<Symbol,std::pair<Symbol,int> > attr_list;
    attrTable.insert(std::pair<Symbol,std::map<Symbol,std::pair<Symbol,int> > >(ordered_class[i]->get_name(),attr_list));
    populate_attrTable(ordered_class[i],ordered_class[i]);  
    int object_size = (attrTable.find(ordered_class[i]->get_name())->second).size();
    str << WORD << "-1" <<endl;
    str << ordered_class[i]->get_name() << PROTOBJ_SUFFIX << LABEL;
    str << WORD << i << endl;
    str << WORD << 3+object_size << endl;
    str << WORD << ordered_class[i]->get_name() << DISPTAB_SUFFIX << endl;
    emit_attributes(ordered_class[i]); 
  }
}


void CgenClassTable::code_init_classes()
{
  for(unsigned i = 0;i<ordered_class.size();i++)
  {
    CgenNodeP cur_class = ordered_class[i];
    Symbol classname = cur_class->get_name();
    cur_classname = classname;
    str<<classname<<CLASSINIT_SUFFIX<<LABEL;

    //Emmiting code for initializing the activation record
    emit_push(FP, str);    //Pushing Frame pointer
    emit_push(SELF, str);   //Pushing the self pointer
    emit_push(RA, str);      //Pushing the return address
    emit_addiu(FP,SP,4,str); // set $fp to a new position
    emit_move(SELF,ACC,str); 
    //Initializing parent classes
    if(classname!=Object)
    {
      str << JAL <<cur_class->get_parentnd()->get_name()->get_string()<< CLASSINIT_SUFFIX <<endl;
    }

    Features fs = cur_class->features;
    for(int i = fs->first(); fs->more(i); i= fs->next(i))
    {
      //Code generation for init expressions
      attr_class *attribute = dynamic_cast<attr_class *>(fs->nth(i));
      if(attribute!=NULL){
        Expression assigned = attribute->init;
        if(assigned->get_type()!=NULL)
        {
          assigned->code(str);
          int off =(attrTable.find(classname)->second).find(attribute->name)->second.second;
          emit_store(ACC,3+off,SELF,str);
        }
      }

    }
    emit_move(ACC,SELF,str);
    emit_pop(RA,str);
    emit_pop(SELF,str);
    emit_pop(FP,str);
    //emit_load(FP,3,SP,str);
    //emit_load(SELF,2,SP,str);
    //emit_load(RA,1,SP,str);
    //emit_addiu(SP,SP,12,str);
    emit_return(str); 
  } 
}

//Emmitting code for each methods for every classes except the Basic classes
void CgenClassTable::emit_methods()
{
  for(unsigned i = 0; i<ordered_class.size();i++)
  {

    cur_classname = ordered_class[i]->get_name();
    if(cur_classname!=Object && cur_classname!=Str && cur_classname!=IO){
      Features fs = ordered_class[i]->features;
      for(int j = fs->first(); fs->more(j); j = fs->next(j))
      {
        method_class * method = dynamic_cast<method_class *>(fs->nth(j));
        if(method!=NULL)
        {

          Expression body = method->expr;
          int count = 0;
          Formals formals = method->formals;

          formalScope.clear();
          //Storing the offsets from the frame pointer of the formals for use in method body. 
          for(int k = formals->first();formals->more(k);k=formals->next(k)){
            //if(cgen_debug) cout<<k<<endl;
            formal_class *formal = dynamic_cast<formal_class *>(formals->nth(k));
            if(formal!=NULL){
              formalScope.push_back(formal->name);
              count++;
            }
          }

          str<< ordered_class[i]->get_name() << METHOD_SEP << method->name << LABEL;
          //Initializing frame pointer and storing  previous frame pointer and return address 
          emit_addiu(SP,SP,-12,str);
          emit_store(FP,3,SP,str);
          emit_store(SELF,2,SP,str);
          emit_store(RA,1,SP,str);
          emit_addiu(FP,SP,4,str);
          emit_move(SELF,ACC,str);

          body->code(str);            //emit code for the body expression
          
          //Poping frame pointer and return address and self pointer
          emit_load(FP,3,SP,str);
          emit_load(SELF,2,SP,str);
          emit_load(RA,1,SP,str);
          emit_addiu(SP,SP,count* 4 + 12,str); // number of arguments + 12
          emit_return(str);
        }
      }
    }
  }
}

void CgenClassTable::code()
{
  if (cgen_debug) cout << "coding global data" << endl;
  code_global_data();

  if (cgen_debug) cout << "choosing gc" << endl;
  code_select_gc();

  if (cgen_debug) cout << "coding constants" << endl;
  code_constants();

//                 Add your code to emit
//                   - prototype objects
//                   - class_nameTab
//                   - dispatch tables
//
/*Modified code starts Here*/  
  if(cgen_debug) cout << "Coding class_nameTab" << endl;
  code_class_nameTab();

  if(cgen_debug) cout << "Coding Object Table" << endl;
  code_class_obj();

  if(cgen_debug) cout << "Coding Disptach table" <<endl;
  code_dispatchTable();

  if (cgen_debug) cout << "Coding Prototype Objects" << endl;
  code_class_protObj();
/*Modified code ends here*/

  if (cgen_debug) cout << "coding global text" << endl;
  code_global_text();


//                 Add your code to emit
//                   - object initializer
//                   - the class methods
//                   - etc...
  if(cgen_debug) cout << "Coding Init for each Classes" << endl;
  code_init_classes();
  
  if(cgen_debug) cout << "Coding Methods for each classes" << endl;
  emit_methods();
}


CgenNodeP CgenClassTable::root()
{
   return probe(Object);
}


///////////////////////////////////////////////////////////////////////
//
// CgenNode methods
//
///////////////////////////////////////////////////////////////////////

CgenNode::CgenNode(Class_ nd, Basicness bstatus, CgenClassTableP ct) :
   class__class((const class__class &) *nd),
   parentnd(NULL),
   children(NULL),
   basic_status(bstatus)
{ 
   stringtable.add_string(name->get_string());          // Add class name to string table
}


//******************************************************************
//
//   Fill in the following methods to produce code for the
//   appropriate expression.  You may add or remove parameters
//   as you wish, but if you do, remember to change the parameters
//   of the declarations in `cool-tree.h'  Sample code for
//   constant integers, strings, and booleans are provided.
//
//*****************************************************************

void assign_class::code(ostream &s) {
  int off;    //Offset for refering the variable
  expr->code(s);    //emmitting code for the initializing expression
  if(letScope.size()!=0){ //First check the scope from the let expressions if any
    for(int i = letScope.size()-1;i>=0;i--)
    {
      if(letScope[i]==name)
      {
        //The variable from the innermost scope will be referred
        if(cgen_debug) cout<<"Variable refrenced from Let\n";
        off = letScope.size()-i;
        emit_store(ACC,off,SP,s);
        return;
      }
    }
  }
  //Then check whether the variable is defined in the formals
  if(formalScope.size()!=0)
  {
    for(unsigned i = 0;i<formalScope.size();i++)
    {
      if(formalScope[i]==name)
      {
        if(cgen_debug) cout<<"Variable refrenced from Formal Arguments\n";
        off =formalScope.size() - i - 1;
        emit_store(ACC,off+3,FP,s);
        return;
      }
    }
  }

  //otherwise search from the attribute table the correct location
  off = (attrTable.find(cur_classname)->second).find(name)->second.second;
  emit_store(ACC,off+3,SELF,s);
}

//Emmitting code for static dispatch expression
void static_dispatch_class::code(ostream &s) {
  int num_of_actuals = 0; //For maintianing the offsets of the let Variables
  int not_void = label_count; // Label if the method is not on void
  int offset;       //Offset for the method in the dispatch table
  Symbol class_type = type_name; 
  if(class_type==SELF_TYPE)
    class_type = cur_classname;

  //Generating the label for the dispatch table
  char *disp1 = class_type->get_string();
  char dispAddress[500];
  strcpy(dispAddress,disp1);
  strcat(dispAddress,"_dispTab");
  label_count++;

  //Code each actuals from  start
  for(int i=actual->first();actual->more(i);i=actual->next(i))
  {
    num_of_actuals++;
    actual->nth(i)->code(s);
    letScope.push_back(No_type);  //For maintaing the offsets of letVariables 
    emit_push(ACC,s);
  }
  
  //Coding the left hand side expression
  expr->code(s);
  
  //If the expression is on void or not
  emit_bne(ACC,ZERO,not_void,s);
  emit_load_address(ACC,"str_const0",s);
  emit_load_imm(T1,1,s);
  emit_jal("_dispatch_abort",s);

  //If the dispach is not on void
  emit_label_def(not_void,s);
  emit_load_address(T1,dispAddress,s);
  offset = (dispatchTable.find(class_type)->second).find(name)->second.second;
  emit_load(T1,offset,T1,s);
  emit_jalr(T1,s);

  //Pop from the letscope to maintain the offset of let variables.
  for(int i = 0; i<num_of_actuals;i++)
    letScope.pop_back();
}

//Emmitting code for disoatch expression
void dispatch_class::code(ostream &s) {
  int num_of_actuals = 0; //Number of actuals as a helper for maintaing the letscopes
  int not_void = label_count;   //if not on void then branch to this label
  int offset;                   //Offset to find the correct method
  Symbol expr_type = expr->get_type();
  if(expr_type==SELF_TYPE)
    expr_type = cur_classname;
  label_count++;

  //Code each actuals then push to the stack
  for(int i=actual->first();actual->more(i);i=actual->next(i))
  {
    num_of_actuals++;
    actual->nth(i)->code(s);
    letScope.push_back(No_type);
    emit_push(ACC,s);
  }

  //Codeing the left hand side expression
  expr->code(s);

  //If the dispatch on void or not
  emit_bne(ACC,ZERO,not_void,s);
  emit_load_address(ACC,"str_const0",s);
  emit_load_imm(T1,1,s);
  emit_jal("_dispatch_abort",s);

  //If the dispatch is not on void
  emit_label_def(not_void,s);
  emit_load(T1,2,ACC,s);
  offset = (dispatchTable.find(expr_type)->second).find(name)->second.second;
  emit_load(T1,offset,T1,s);
  emit_jalr(T1,s);

  //Pop all the unnecessary actuals after the need is over
  for(int i = 0; i<num_of_actuals;i++)
    letScope.pop_back();
}

//Emmiting code for if-then else fi  expression
void cond_class::code(ostream &s) {
  int truelabel = label_count;    //Truelabel: Jump if the predicate evaluates to true
  label_count++;
  int falselabel = label_count;   //False label: Jump if the predicate is false
  label_count++;

  pred->code(s);                  //Code the predicate to fetch its value

  emit_fetch_int(T1,ACC,s);       //Fetch the value from the correct location
  emit_beqz(T1,falselabel,s);
  then_exp->code(s);              //If true
  emit_branch(truelabel,s);
  emit_label_def(falselabel,s);   
  else_exp->code(s);              //If false
  emit_label_def(truelabel,s);
}

void loop_class::code(ostream &s) {
  int truelabel = label_count;
  label_count++;
  int falselabel = label_count;
  label_count++;

  //True branch
  emit_label_def(truelabel,s);
  pred->code(s);
  emit_load(T1,3,ACC,s);
  emit_beqz(T1,falselabel,s);  //Exit the while loop if false
  body->code(s);
  emit_branch(truelabel,s);    //Continue the while loop
  
  emit_label_def(falselabel,s);
  emit_move(ACC,ZERO,s);      //Put the value zero to the accumulator as the evaluted value is always zero
}

void typcase_class::code(ostream &s) {
}

void block_class::code(ostream &s) {
  //Code each expression
  for(int i = body->first(); body->more(i); i = body->next(i))
  {
    body->nth(i)->code(s);
  }
}

void let_class::code(ostream &s) {
  //If there is no initialization then put the default values in the accumulator
  if(init->get_type() == NULL)
  {
    if(type_decl == Int)
      emit_load_int(ACC,inttable.lookup_string("0"),s);
    else if(type_decl == Bool)
      emit_load_bool(ACC,BoolConst(false),s);
    else if(type_decl == Str)
      emit_load_string(ACC,stringtable.lookup_string(""),s);
    else
      emit_load_imm(ACC,0,s);
  }

  else
  {
    //Else code the initialization expression
    if(cgen_debug) cout<<"Let has an assignment" <<endl;
    init->code(s);
  }

  //Push the variable onto the stack for future reference inside the stack
  letScope.push_back(identifier);     //For maintiang the offset for referencing from the stack
  emit_push(ACC,s);
  body->code(s);      //Code the body of let expression 
  emit_addiu(SP,SP,4,s);      //pop the variable since it is not needed and should not be accessed
  letScope.pop_back();      //Not needing outside its body
}

/*
The following is the logic for all the binary arithmetic operators
1. Evaluate the left expression and push onto the stack
2. Evaluate the right hand side and move the value to a temporary register
3. allocate some space for the current expression
4. Do the corresponding operation
*/
void plus_class::code(ostream &s) {
  e1->code(s); 
  letScope.push_back(No_type);
  emit_push(ACC,s);  
  e2->code(s);       
  emit_fetch_int(T2,ACC,s); 
  emit_jal("Object.copy",s);
  emit_pop(T1,s);        
  emit_fetch_int(T1,T1,s);
  emit_add(T1,T1,T2,s);
  emit_store_int(T1,ACC,s);
  letScope.pop_back();
}

void sub_class::code(ostream &s) {
  e1->code(s);
  letScope.push_back(No_type);
  emit_push(ACC,s);
  e2->code(s);
  emit_fetch_int(T2,ACC,s);
  emit_jal("Object.copy",s);
  emit_pop(T1,s);
  emit_fetch_int(T1,T1,s);
  emit_sub(T1,T1,T2,s);
  emit_store_int(T1,ACC,s); 
  letScope.pop_back();
}

void mul_class::code(ostream &s) {
  e1->code(s);
  letScope.push_back(No_type);
  emit_push(ACC,s);
  e2->code(s);
  emit_fetch_int(T2,ACC,s);
  emit_jal("Object.copy",s);
  emit_pop(T1,s);
  emit_fetch_int(T1,T1,s);
  emit_mul(T1,T1,T2,s);
  emit_store_int(T1,ACC,s);
  letScope.pop_back();
}

void divide_class::code(ostream &s) {
  e1->code(s);
  letScope.push_back(No_type);
  emit_push(ACC,s);
  e2->code(s);
  emit_fetch_int(T2,ACC,s);
  emit_jal("Object.copy",s);
  emit_pop(T1,s);
  emit_fetch_int(T1,T1,s);
  emit_div(T1,T1,T2,s);
  emit_store_int(T1,ACC,s);
  letScope.pop_back();
}

//Take the current value of the object in a temporary register
//Allocate some space from the heap for the new object
//And negate the value of the register and put it back in the correct loaction
void neg_class::code(ostream &s) {
  e1->code(s);
  emit_push(ACC,s);
  letScope.push_back(No_type);
  emit_jal("Object.copy",s);
  emit_pop(T2,s);
  letScope.pop_back();
  emit_fetch_int(T2,T2,s);
  emit_neg(T2,T2,s);
  emit_store_int(T2,ACC,s); 
}

/*The follwoing is the login for less than and less than equal to expressions
1. Evaluate the left expression and push onto the stack
2. Evaluates the second expression and retreive it  value.
3. Retreive the value of the left expression from the stack
4. Compare and branch
*/
void lt_class::code(ostream &s) {
  int truelabel = label_count;
  label_count++;
  e1->code(s);
  letScope.push_back(No_type);
  emit_push(ACC,s);
  e2->code(s);
  emit_pop(T1,s);
  letScope.pop_back();
  emit_fetch_int(T1,T1,s);
  emit_fetch_int(T2,ACC,s);
  emit_load_bool(ACC,truebool,s);
  emit_blt(T1,T2,truelabel,s);
  emit_load_bool(ACC,falsebool,s);
  emit_label_def(truelabel,s);
}


//For the equal first check if the they point to the same locations
// If they are not pointing to the same location then check if they are of basic tyoes
//If they are then call equality_test else return false as the result
void eq_class::code(ostream &s) {
  int truelabel = label_count;
  label_count++;
  e1->code(s);
  emit_push(ACC,s);
  letScope.push_back(No_type);
  e2->code(s);
  emit_pop(T1,s);
  letScope.pop_back();
  emit_move(T2,ACC,s);
  emit_load_bool(ACC,truebool,s);
  emit_beq(T2,T1,truelabel,s);
  Symbol expr = e1->get_type();
  emit_load_bool(ACC,falsebool,s);
  if(expr == Int || expr == Bool || expr == Str )
  {
    emit_load_bool(ACC,truebool,s);
    emit_load_bool(A1,falsebool,s);
    emit_jal("equality_test",s);
  }
  emit_label_def(truelabel,s);
}


void leq_class::code(ostream &s) {
  int truelabel = label_count;
  label_count++;
  e1->code(s);
  letScope.push_back(No_type);
  emit_push(ACC,s);
  e2->code(s);
  emit_pop(T1,s);
  letScope.pop_back();
  emit_fetch_int(T1,T1,s);
  emit_fetch_int(T2,ACC,s);
  emit_load_bool(ACC,truebool,s);
  emit_bleq(T1,T2,truelabel,s);
  emit_load_bool(ACC,falsebool,s);
  emit_label_def(truelabel,s);
}

//For complement class first check if the value is zero. And put the reverse value in the accumulator 
void comp_class::code(ostream &s) {
  int truelabel = label_count;
  label_count++;
  e1->code(s);
  emit_fetch_int(T1,ACC,s);
  emit_load_bool(ACC,truebool,s);
  emit_beqz(T1,truelabel,s);
  emit_load_bool(ACC,falsebool,s);
  emit_label_def(truelabel,s);
}

void int_const_class::code(ostream& s)  
{
  //
  // Need to be sure we have an IntEntry *, not an arbitrary Symbol
  //
  emit_load_int(ACC,inttable.lookup_string(token->get_string()),s);
}

void string_const_class::code(ostream& s)
{
  emit_load_string(ACC,stringtable.lookup_string(token->get_string()),s);
}

void bool_const_class::code(ostream& s)
{
  emit_load_bool(ACC, BoolConst(val), s);
}

//There are three lines of code for the new 
//First put the pointer of the proto type object of the type to the accumulator 
//Then call Object.copy for allocating space
//Then call the init function of that class
void new__class::code(ostream &s) {
  if(type_name == SELF_TYPE){}
  else{
  char *classname = type_name->get_string();
  char proto[500] = "";
  char init[500] = "";
  strcpy(proto,classname);
  strcpy(init,classname);
  strcat(proto,"_protObj");
  strcat(init,"_init");
  emit_load_address(ACC,proto,s);
  emit_jal("Object.copy",s);
  emit_jal(init,s);
  }  
}

//First evalute the expression.
//Then check whether it points to the correct location
//If it points then put true else false
void isvoid_class::code(ostream &s) {
  int truelabel = label_count;
  label_count++;
  int falselabel = label_count;
  label_count++;
  e1->code(s);
  emit_beqz(ACC,truelabel,s);
  emit_load_bool(ACC,falsebool,s);
  emit_branch(falselabel,s);
  emit_label_def(truelabel,s);
  emit_load_bool(ACC,truebool,s);
  emit_label_def(falselabel,s);
}

void no_expr_class::code(ostream &s) {
}

//If the object is self then put the pointer of the current class prototype as the result
//Else check in the let scope for that variable, then check in the formal paramters then 
//for attributes and load the correct address 
void object_class::code(ostream &s) {
  int off;
  if(name == self)
  {
    emit_move(ACC,SELF,s);
    return;
  }

  if(letScope.size()!=0){
    for(int i = letScope.size()-1;i>=0;i--)
    {
      if(letScope[i]==name)
      {
        if(cgen_debug) cout<<"Variable refrenced from Let\n";
        off = letScope.size()-i;
        emit_load(ACC,off,SP,s);
        return;
      }
    }
  }

  if(formalScope.size()!=0)
  {
    for(unsigned i =0; i<formalScope.size();i++)
    {
      if(formalScope[i] == name)
      {
        off = formalScope.size()-i-1;
        emit_load(ACC,off+3,FP,s);
        return;
      }
    }
  }
  off = (attrTable.find(cur_classname)->second).find(name)->second.second;
  emit_load(ACC,off+3,SELF,s);
}

