/**
 * This file contains the standard library of the vm.
 */

struct Env;

void loadIOLib(Env &env);

void loadMathLib(Env &env);

void loadLogicalLib(Env &env);

void loadStdLib(Env &env);
