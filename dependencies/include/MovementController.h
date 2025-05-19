class MovementController {
private:
    InputHandler& ip;
    Mat4& view;
    float& dt;
    float movementSpeed = 5.0f;

public:
    MovementController(InputHandler& _ip, Mat4& _view, float& _dt) 
        : ip(_ip), view(_view), dt(_dt) {};
        
    void move(){

        ip.listen();
        inputs input = ip.getInputs();

        float movement = movementSpeed * dt;
        if (input.w == true)
            view.translate(0, 0, movement);
        if (input.a == true)
            view.translate(movement, 0, 0);
        if (input.s == true)
            view.translate(0, 0, -movement);
        if (input.d == true)
            view.translate(-movement, 0, 0);

        if (input.space == true)
            view.translate(0, -movement, 0);
        if (input.shift == true)
            view.translate(0, movement, 0);

        /*
        if (input.lArrow == true)
            view.rotate(0, -movement, 0);
        if (input.rArrow == true)
            view.rotate(0, movement, 0);
        if (input.uArrow == true)
            view.rotate(-movement, 0, 0);
        if (input.dArrow == true)
            view.rotate(movement, 0, 0);

        */
       
        //  Calcaulte angles blah blah blah
    }
};