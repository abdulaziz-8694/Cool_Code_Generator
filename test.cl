class A inherits IO{
	c:Int;
	a():Object{
		self
	};
	b():Object{
		self
	};
};
class Main inherits A{

	a():Object{
		self
	};
	main():Object{a()};
};