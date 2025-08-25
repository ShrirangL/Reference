\begin{verbatim}
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main() {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 target   = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 model      = glm::mat4(1.0f); // Identity
    glm::mat4 view       = glm::lookAt(position, target, up);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.f / 3.f, 0.1f, 100.0f);

    glm::mat4 mvp = projection * view * model;

    const float* data = glm::value_ptr(mvp);
    for (int i = 0; i < 16; ++i) {
        std::cout << data[i] << ((i % 4 == 3) ? "\n" : "\t");
    }

    return 0;
}
