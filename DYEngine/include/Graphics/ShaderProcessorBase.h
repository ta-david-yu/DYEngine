#pragma once

#include "Graphics/Shader.h"

#include <vector>
#include <string>
#include <regex>

namespace DYE::ShaderProcessor
{
    class ShaderProcessorBase
    {
    public:
        explicit ShaderProcessorBase(std::string name);
        virtual ~ShaderProcessorBase() = default;

        std::string GetName() const { return m_Name; }

        /// Specify what directives should be ignored in the shader type parsing phase.
        /// For instance, UniformProperty directive #property should be omitted for the shader source.
        /// \return a list of c-strings to ignored
        virtual std::vector<std::string> GetDirectivesToIgnoreInShaderTypeParsePhase() const
        {
            // By default, it's empty.
            return {};
        };

        virtual void OnBegin(ShaderProgram &shaderProgram) = 0;
        virtual void OnPreShaderTypeParse(std::string &programSource) {};
        virtual void OnPostShaderTypeParse(ShaderProgram::ShaderTypeParseResult &parseResult) {};
        virtual void OnPreShaderCompilation(ShaderType shaderType, std::string &shaderSource) {};
        virtual void OnPostShaderCompilation(ShaderType shaderType, ShaderProgram::ShaderCompilationResult &compilationResult) {};
        virtual void OnEnd(ShaderProgram &shaderProgram) = 0;

    protected:
        static std::string getFirstTokenFromLine(const std::string &line)
        {
            auto firstToken = line.substr(0, line.find(' '));
            return firstToken;
        }

        static std::vector<std::string> splitLineIntoTokensBySpace(std::string const &line, uint32_t numberOfTokensNeeded)
        {
            std::regex const directiveRegexPattern(R"(\s+)");
            std::vector<std::string> tokens;
            int numberOfMatches = 0;
            std::smatch matchObject;

            std::string lineToBeProcessed = line;
            while (std::regex_search(lineToBeProcessed, matchObject, directiveRegexPattern))
            {
                // Add the token
                std::string const token = lineToBeProcessed.substr(0, matchObject.position());
                tokens.emplace_back(token);

                // Skip the matched string (whitespaces)
                lineToBeProcessed = lineToBeProcessed.substr(matchObject.position() + matchObject.length());

                numberOfMatches++;
                if (numberOfMatches >= numberOfTokensNeeded - 1)
                {
                    // #(numberOfTokensNeeded - 1) matches would already give us #numberOfTokensNeeded tokens.
                    // Push the rest of the string as the final token.
                    tokens.emplace_back(lineToBeProcessed);
                    break;
                }
            }

            return std::move(tokens);
        }
    private:
        std::string m_Name;
    };
}
