# My attempt to implement minimalistic flexbox layout

I wanted to implement my own DOM (document object model) based UI toolkit.

```c++
    class BaseNode
    {
    public:
        virtual ~BaseNode() = default;

        void reLayout(Layout inLayout) { layout = inLayout; propagateLayout(); };
        void input(InputContext& inCtx);
        void draw();


        [[nodiscard]] virtual qm::Vec2i getRequiredSize(qm::Vec2i inAvailable) const = 0;

        Layout layout {};

    protected:
        virtual void propagateLayout() {};
        virtual void propagateInput(InputContext& inCtx) {};
        virtual void handleDraw() const = 0;

        virtual void onHover() {};
        virtual void onDeHover() {};

        bool debugHovered {false};
    };
```
