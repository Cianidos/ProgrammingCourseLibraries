#pragma once
#include <iostream> 
#include <functional>
#include <list>
#include <istream>
#include <optional>

#include "io_script.h"

namespace adio
{
    class script_builder
    {
        std::shared_ptr<io_script> target;
    public:
        script_builder(std::istream& input = std::cin, std::ostream& output = std::cout)
            : target(std::make_shared<io_script>(input, output))
        {
        }
        using sb_ref = script_builder&;

        sb_ref add_text(const std::string& text)
        {
            target->add_step(
                std::make_shared<io_script::write<std::string>>(target, text)
            );
            return *this;
        }
        sb_ref add_endl()
        {
            target->add_step(
                std::make_shared<io_script::write<std::string>>(target, "\n")
            );
            return *this;
        }
        template<has_output_operator Write_Ty>
        sb_ref add_write_object(Write_Ty obj)
        {
            target->add_step(
                std::make_shared<io_script::write<Write_Ty>>(target, obj)
            );
            return *this;
        }

        sb_ref add_yn_q(std::string q_text, std::function<void(bool)> handler)
        {
            target->add_step(
                std::make_shared<io_script::yn_code>(target, q_text, handler)
            );
            return *this;
        }

        template<has_input_operator  Answer_Ty>
        sb_ref add_choose_q
        (
            std::optional<std::string> q_text,
            std::optional<std::list<std::string>> variants,
            std::function<void(Answer_Ty)> handler
        )
        {
            target->add_step(std::make_shared<io_script::choose<Answer_Ty>>(
                target,
                q_text,
                variants,
                handler
                ));
            return *this;
        }

        sb_ref add_choose_q_enumerated
        (
            std::optional<std::string> q_text,
            std::optional<std::list<std::string>> variants,
            std::vector<std::function<void()>> handlers
        )
        {
            target->add_step(std::make_shared<io_script::choose_enumerated<std::list>>(
                target,
                q_text,
                variants,[=](size_t index)
                {
                    handlers[index]();
                }
                ));
            return *this;
        }

        template<has_input_operator  Input_Ty>
        sb_ref read_data(Input_Ty& obj)
        {
            target->add_step(
                std::make_shared<io_script::read<Input_Ty>>(target, [&](Input_Ty obj2)
                {
                    obj = obj2;
                })
            );
            return *this;
        }

        sb_ref execute_code(std::function<void(void)> code)
        {
            target->add_step(
                std::make_shared<io_script::execute_code>(target, code)
            );
            return *this;
        }

        sb_ref start_script(std::shared_ptr<io_script> script)
        {
            target->add_step(
                std::make_shared<io_script::execute_code>(target, [&]()
                    {
                        script->start();
                    })
            );
            return *this;
        }

        std::shared_ptr<io_script> build()
        {
            return target;
        }
    };
}

