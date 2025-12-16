#ifndef BLACKHOLESIMULATOR_SHADER_PROGRAM_H
#define BLACKHOLESIMULATOR_SHADER_PROGRAM_H


class shader_program {
public:
    void use() const;

protected:
    const unsigned int ID;
    shader_program(const unsigned int ID) : ID(ID) {}
};


#endif //BLACKHOLESIMULATOR_SHADER_PROGRAM_H