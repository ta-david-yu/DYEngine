#pragma once

#include <string>
#include <vector>
#include <memory>

namespace DYE::DYEditor
{
    class UndoOperationBase
    {
    public:
        virtual void Undo() = 0;
        virtual void Redo() = 0;
        const char *GetDescription()
        {
            return Description;
        }

        virtual bool HasTooltip() const { return false; }

        virtual void DrawTooltip() const {}

        virtual ~UndoOperationBase() = default;

        char Description[128] = "Unnamed Operation";
    };

    class GroupUndoOperation final : public UndoOperationBase
    {
    public:
        void Undo() override;
        void Redo() override;

        bool HasTooltip() const override { return true; }
        void DrawTooltip() const override;

        std::vector<std::unique_ptr<UndoOperationBase>> OperationCollection;
    };
}