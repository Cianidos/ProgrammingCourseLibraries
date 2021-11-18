#include <iostream>

template<typename Interface>
struct Decorator : public Interface
{
	Interface* other_ = nullptr;
	Interface* decorator_init_(Interface* other)
	{
		other_ = other;
		return this;
	}
};
template<typename Wrapper, typename Interface>
concept IsDecorator = requires(Wrapper w, Interface * f)
{
	{ w.decorator_init_(f) } -> std::same_as<Interface*>;
};
template<typename Interface, IsDecorator<Interface> Wrapper, IsDecorator<Interface> WrapperTwo,
	IsDecorator<Interface> ... Other>
	Decorator<Interface>* decorate(Interface* to)
{
	Wrapper* t = new Wrapper();
	t->decorator_init_(to);
	return decorate<Interface, WrapperTwo, Other...>(t);
}
template<typename Interface, IsDecorator<Interface> Wrapper>
Decorator<Interface>* decorate(Interface* to)
{
	Wrapper* t = new Wrapper();
	t->decorator_init_(to);
	return t;
}


struct ISpeakable
{
	virtual std::string speak() = 0;
};
struct Human : public ISpeakable
{
	std::string speak() override
	{
		return " Hello";
	}
};
struct TalkativeHuman : public Decorator<ISpeakable>
{
	std::string speak() override
	{
		return other_->speak() + "Hmmmm";
	}
};
struct ZZZZZHuman : public Decorator<ISpeakable>
{
	std::string speak() override
	{
		return other_->speak() + "ZZZ";
	}
};

struct LoggedHuman : public Decorator<ISpeakable>
{
	std::string speak() override
	{
		auto s = other_->speak();
		std::cerr << s << std::endl;
		return s;
	}
};
struct TalkativeHuman2 : public ISpeakable
{
	ISpeakable* other;
	TalkativeHuman2(ISpeakable* other) : other(other)
	{ }
	std::string speak() override
	{
		return other->speak() + "Hmmmm";
	}
};
struct ZZZZZHuman2 : public ISpeakable
{
	ISpeakable* other;
	ZZZZZHuman2(ISpeakable* other) : other(other)
	{ }
	std::string speak() override
	{
		return other->speak() + "ZZZ";
	}
};
int main()
{
	ISpeakable* h = new Human();

	auto ttt = decorate<ISpeakable, ZZZZZHuman, TalkativeHuman, TalkativeHuman, LoggedHuman>(h);
	std::cout << ttt->speak();
}

