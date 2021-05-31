#pragma once
#include <istream>
#include <ostream>
#include <memory>
#include <set>
#include <functional>
#include <string>
#include <list>
#include <optional>
#include <utility>

#include "script_builder.h"
#include "utils.h"


// advanced io
namespace adio
{
    /// <summary>
    /// Script - ready io program
    /// Composed from nodes "base" is a base node
    /// </summary>
    class io_script
    {
    public:
        /// <summary>
        /// Base scripts node 
        /// </summary>
        class base
        {
        public:
            std::shared_ptr<io_script> owner;
            std::shared_ptr<base> next;

            base(std::shared_ptr<io_script> owner)
                : owner(owner)
            {}

            virtual void do_work() {}
            virtual void execute() {
                do_work();
                if (next)
                {
                    next->execute();
                }
            }
        };

        class none : public base
        {
        public:
            none(std::shared_ptr<io_script> owner)
                : base(owner)
            {}
            void do_work() override {}
        };

        template<has_output_operator Write_Ty = std::string>
        class write : public base
        {
        public:
            Write_Ty text;
            write(std::shared_ptr<io_script> owner, Write_Ty text)
                : base(owner), text(text)
            {
            }
            void do_work() override
            {
                owner->output << text << std::endl;
            }
        };

        template<has_input_operator Read_Ty>
        class read : public base
        {
        public:
            Read_Ty obj;
            std::function<void(Read_Ty)> handler;
            read(std::shared_ptr<io_script> owner, std::function<void(Read_Ty)> handler)
                : base(owner), obj(), handler(handler)
            { }
            void do_work() override
            {
                owner->input >> obj;
                handler(obj);
            }
        };


        class yn_code : public base
        {
        public:
            std::string q_text;
            std::function<void(bool)> handler;
            yn_code(std::shared_ptr<io_script> owner,
                std::string q_text,
                std::function<void(bool)> handler)
                : base(owner), q_text(q_text), handler(std::move(handler))
            {
            }
            void do_work() override
            {
                auto message =
                    std::make_shared<write<std::string>>(owner, q_text + "(Y/n)");
                auto reading =
                    std::make_shared<read<char>>(owner, [&](char answer)
                        {
                            handler(std::set{ 'Y','y' }.count(answer));
                        });
                message->next = reading;
                reading->next = next;
                next = message;
            }
        };

        template<has_input_operator Answer_Ty, template<typename T = std::string, typename ...Arg> typename Container = std::list>
        class choose : public base
        {
        public:
            std::optional<std::string> q_text;
            std::optional<Container<std::string>> variants;
            std::function<void(Answer_Ty)> handler;

            choose(
                std::shared_ptr<io_script> owner,
                std::optional<std::string> q_text,
                std::optional<Container<std::string>> variants,
                std::function<void(Answer_Ty)> handler
            ) : base(owner), q_text(q_text), variants(variants), handler(std::move(handler))
            {}
            void do_work() override
            {
                auto current = std::make_shared<base>(owner);
                auto first = current;
                if (q_text)
                {
                    current->next = std::make_shared<write<std::string>>(owner, *q_text);
                    current = current->next;
                }
                if (variants)
                {
                    for (auto variant : *variants)
                    {
                        current->next = std::make_shared<write<std::string>>(owner, variant);
                        current = current->next;
                    }
                }
                current->next = std::make_shared<read<Answer_Ty>>(owner, handler);
                current = current->next;
                current->next = next;
                next = move(first);
            }
        };

        template<template <typename T = std::string, typename ...Arg> typename Container = std::list>
        class choose_enumerated : public choose<size_t, Container>
        {
            using super = choose<size_t, Container>;
        public:
            choose_enumerated(
                std::shared_ptr<io_script> owner,
                std::optional<std::string> q_text,
                std::optional<Container<std::string>> variants,
                std::function<void(size_t)> handler
            ) : super(owner, q_text, variants, handler)
            {}
            void do_work() override
            {
                if (this->variants)
                {
                    int i = 0;
                    for (auto& s : *this->variants)
                    {
                        s = std::to_string(i) + ") " + s;
                        ++i;
                    }
                }
                super::do_work();
            }
        };

        class execute_code : public base
        {
            public:
            std::function<void(void)> code;
            execute_code(std::shared_ptr<io_script> owner, std::function<void(void)> code)
                : base(owner), code(std::move(code))
            {
            }
            void do_work() override
            {
                code();
            }
        };


        io_script(std::istream& input, std::ostream& output)
            :input(input), output(output)
        {
        }
        void start()
        {
            head->execute();
        }
        void add_step(std::shared_ptr<base> step)
        {
            if (!head)
            {
                tail = step;
                head = tail;
            }
            else {
                tail->next = step;
                tail = tail->next;
            }
        }

        friend base;

    private:
        std::istream& input;
        std::ostream& output;
        std::shared_ptr<base> head;
        std::shared_ptr<base> tail;
    };

}
