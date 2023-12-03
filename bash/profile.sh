
g++ -O0 -pg main.cpp -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
./a.out
gprof a.out gmon.out | gprof2dot -s -w | dot -Gdpi=300 -Tpng -o Profile.png