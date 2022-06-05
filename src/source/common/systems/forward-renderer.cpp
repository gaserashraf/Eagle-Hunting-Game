#include "forward-renderer.hpp"
#include "../mesh/mesh-utils.hpp"
#include "../texture/texture-utils.hpp"
#include "../components/light.hpp"
#include "../components/skyLight.hpp"
#include <glm/gtx/euler_angles.hpp>

namespace our {

 struct {
        glm::vec3 fog_color = {0.86274,0.86274 , 0.86274};
        float fog_power = 45.0f;
        float fog_distance = 1.5f;
    } fog{};
    void ForwardRenderer::initialize(glm::ivec2 windowSize, const nlohmann::json& config){
        // First, we store the window size for later use
        this->windowSize = windowSize;

        // Then we check if there is a sky texture in the configuration
        if(config.contains("sky")){
            // First, we create a sphere which will be used to draw the sky
            this->skySphere = mesh_utils::sphere(glm::ivec2(16, 16));
            
            // We can draw the sky using the same shader used to draw textured objects
            ShaderProgram* skyShader = new ShaderProgram();
            skyShader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
            skyShader->attach("assets/shaders/textured.frag", GL_FRAGMENT_SHADER);
            skyShader->link();
            
            //TODO: (Req 9) Pick the correct pipeline state to draw the sky
            // Hints: the sky will be draw after the opaque objects so we would need depth testing but which depth funtion should we pick?
            // We will draw the sphere from the inside, so what options should we pick for the face culling.
            PipelineState skyPipelineState{};  
            skyPipelineState.faceCulling.enabled=true;  
            skyPipelineState.faceCulling.culledFace=GL_FRONT;
            skyPipelineState.faceCulling.frontFace=GL_CCW;

            skyPipelineState.depthTesting.enabled = true;
            skyPipelineState.depthTesting.function = GL_LEQUAL;
            
            // Load the sky texture (note that we don't need mipmaps since we want to avoid any unnecessary blurring while rendering the sky)
            std::string skyTextureFile = config.value<std::string>("sky", "");
            Texture2D* skyTexture = texture_utils::loadImage(skyTextureFile, false);

            // Setup a sampler for the sky 
            Sampler* skySampler = new Sampler();
            skySampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_WRAP_S, GL_REPEAT);
            skySampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Combine all the aforementioned objects (except the mesh) into a material 
            this->skyMaterial = new TexturedMaterial();
            this->skyMaterial->shader = skyShader;
            this->skyMaterial->texture = skyTexture;
            this->skyMaterial->sampler = skySampler;
            this->skyMaterial->pipelineState = skyPipelineState;
            this->skyMaterial->tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            this->skyMaterial->alphaThreshold = 1.0f;
            this->skyMaterial->transparent = false;
        }

        // Then we check if there is a postprocessing shader in the configuration
        if(config.contains("postprocess")){
            //TODO: (Req 10) Create a framebuffer

            glGenFramebuffers(1, &postprocessFrameBuffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);
            //TODO: (Req 10) Create a color and a depth texture and attach them to the framebuffer
            // Hints: The color format can be (Red, Green, Blue and Alpha components with 8 bits for each channel).
            // The depth format can be (Depth component with 24 bits).
            int RT_W = windowSize[0];
            int RT_H = windowSize[1];
            colorTarget = new our::Texture2D();
            // creating new Texture ; 
            GLuint colorRT = colorTarget->getOpenGLName();
            // get the name of the texture ; 
            colorTarget->bind();
            // bind the texture ; 
            int levels = (int)glm::floor(glm::log2((float)glm::max(RT_W, RT_H))) + 1;
            //Compute the no. of miplevels and pass it to glTexStorage2D:
            glTexStorage2D(GL_TEXTURE_2D, levels, GL_RGBA8, RT_W, RT_H);
            
            depthTarget = new our::Texture2D();
            GLuint depthRT = depthTarget->getOpenGLName();
            depthTarget->bind();
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32, RT_W, RT_H);
            // same as above for the depth but instead pass 1 for the levels 

            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorRT, 0);
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthRT, 0);
            //TODO: (Req 10) Unbind the framebuffer just to be safe
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            // Create a vertex array to use for drawing the texture
            glGenVertexArrays(1, &postProcessVertexArray);

            // Create a sampler to use for sampling the scene texture in the post processing shader
            Sampler* postprocessSampler = new Sampler();
            postprocessSampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            // Smooth things out by using a different minification parameter (GL_NEAREST_MIPMAP_LINEAR)
            postprocessSampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            postprocessSampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Create the post processing shader
            ShaderProgram* postprocessShader = new ShaderProgram();
            // create the new shader for the vertex shader 
            postprocessShader->attach("assets/shaders/fullscreen.vert", GL_VERTEX_SHADER);
            // attach it . 
            postprocessShader->attach(config.value<std::string>("postprocess", ""), GL_FRAGMENT_SHADER);
            // attach fragment shader 
            postprocessShader->link();
            // link shaders to the programs .

            // Create a post processing material
            postprocessMaterial = new TexturedMaterial();

            postprocessMaterial->shader = postprocessShader;
            // assign the sahder 
            postprocessMaterial->texture = colorTarget;
            // assign the target color 
            postprocessMaterial->sampler = postprocessSampler;
            // assign the sampler of the texutre 
            // The default options are fine but we don't need to interact with the depth buffer
            // so it is more performant to disable the depth mask
            postprocessMaterial->pipelineState.depthMask = false;
        }
    }

    void ForwardRenderer::destroy(){
        // Delete all objects related to the sky
        if(skyMaterial){
            delete skySphere;
            delete skyMaterial->shader;
            delete skyMaterial->texture;
            delete skyMaterial->sampler;
            delete skyMaterial;
        }
        // Delete all objects related to post processing
        if(postprocessMaterial){
            glDeleteFramebuffers(1, &postprocessFrameBuffer);
            glDeleteVertexArrays(1, &postProcessVertexArray);
            delete colorTarget;
            delete depthTarget;
            delete postprocessMaterial->sampler;
            delete postprocessMaterial->shader;
            delete postprocessMaterial;
        }
    }
    void ForwardRenderer::handleLights(World* world, ShaderProgram* shader){
        //first we get all light entities from the world
        std::vector<Entity*> lights;
        Entity * skyLit;
        for(auto entity : world->getEntities()){
            Light *light = entity->getComponent<Light>();
            skyLight *sLight = entity->getComponent<skyLight>();
            if(light){
                lights.push_back(entity);
            }
            if(sLight){
                skyLit = entity;
            }
        }
        //then we send the lights to the shader
        int numLights = (int)lights.size();
        //std::cout<<"numLights: "<<numLights<<std::endl;
        shader->set("light_count", numLights);
        for(int i = 0; i < numLights; i++){
            Light *light = lights[i]->getComponent<Light>();
            std::string lightString = "lights[" + std::to_string(i) + "]";
            glm::vec3 rotation  = lights[i]->localTransform.rotation;
            glm::vec3 position  = lights[i]->localTransform.position;
            glm::vec3 direction = glm::yawPitchRoll(rotation[1], rotation[0], rotation[2])*glm::vec4(0,-1,0,0);
            
            shader->set(lightString + ".position",  position);
            shader->set(lightString + ".direction", direction);
            shader->set(lightString + ".type", light->lightType);
            shader->set(lightString + ".diffuse", light->diffuse);
            shader->set(lightString + ".specular",light->specular);
            shader->set(lightString + ".attenuation",light->attenuation);
            shader->set(lightString + ".cone_angles", glm::vec2(glm::radians(light->cone_angles.x), glm::radians(light->cone_angles.y)));
            //debug
            // std::cout<<"light found"<<std::endl;
            // std::cout<<"light type: "<<light->lightType<<std::endl;
            // std::cout<<"light position: "<<position[0]<<" " <<position[1]<<" "<<position[2]<<std::endl;
            // std::cout<<"light direction: "<<direction[0]<<" " <<direction[1]<<" "<<direction[2]<<std::endl;
            // std::cout<<"light diffuse: "<<light->diffuse[0]<<" " <<light->diffuse[1]<<" "<<light->diffuse[2]<<std::endl;
            // std::cout<<"light specular: "<<light->specular[0]<<" " <<light->specular[1]<<" "<<light->specular[2]<<std::endl;
            // std::cout<<"light attenuation: "<<light->attenuation[0]<<" " <<light->attenuation[1]<<" "<<light->attenuation[2]<<std::endl;
            // std::cout<<"light cone angles: "<<light->cone_angles.x<<" " <<light->cone_angles.y<<std::endl;
        }
        skyLight *sLight = skyLit->getComponent<skyLight>();
        if(sLight){
            shader->set("sky.top", sLight->topLight);
            shader->set("sky.middle", sLight->middleLight);
            shader->set("sky.bottom", sLight->topLight);
            //std::cout<<"sky light found"<<std::endl;
            //std::cout<<"sky top: "<<sLight->topLight[0]<<" " <<sLight->topLight[1]<<" "<<sLight->topLight[2]<<std::endl;
            //std::cout<<"sky middle: "<<sLight->middleLight[0]<<" " <<sLight->middleLight[1]<<" "<<sLight->middleLight[2]<<std::endl;
            //std::cout<<"sky bottom: "<<sLight->bottomLight[0]<<" " <<sLight->bottomLight[1]<<" "<<sLight->bottomLight[2]<<std::endl;
        }
    }
    void ForwardRenderer::render(World* world){
        // First of all, we search for a camera and for all the mesh renderers
        CameraComponent* camera = nullptr;
        opaqueCommands.clear();
        transparentCommands.clear();
        for(auto entity : world->getEntities()){
            // If we hadn't found a camera yet, we look for a camera in this entity
            if(!camera) camera = entity->getComponent<CameraComponent>();
            // If this entity has a mesh renderer component
            if(auto meshRenderer = entity->getComponent<MeshRendererComponent>(); meshRenderer){
                // We construct a command from it
                RenderCommand command;
                command.localToWorld = meshRenderer->getOwner()->getLocalToWorldMatrix();
                command.center = glm::vec3(command.localToWorld * glm::vec4(0, 0, 0, 1));
                command.mesh = meshRenderer->mesh;
                command.material = meshRenderer->material;
                // if it is transparent, we add it to the transparent commands list
                if(command.material->transparent){
                    transparentCommands.push_back(command);
                } else {
                // Otherwise, we add it to the opaque command list
                    opaqueCommands.push_back(command);
                }
            }
        }

        // If there is no camera, we return (we cannot render without a camera)
        if(camera == nullptr) return;

        // a
        //TODO: (Req 8) Modify the following line such that "cameraForward" contains a vector pointing the camera forward direction
        // HINT: See how you wrote the CameraComponent::getViewMatrix, it should help you solve this one
        glm::vec3 cameraForward = camera->getViewMatrix()[0];
        std::sort(transparentCommands.begin(), transparentCommands.end(), [cameraForward](const RenderCommand& first, const RenderCommand& second){
            //TODO: (Req 8) Finish this function
            // HINT: the following return should return true "first" should be drawn before "second". 
            return (first.localToWorld[2][2]<second.localToWorld[2][2]);
        });

        //TODO: (Req 8) Get the camera ViewProjection matrix and store it in VP
        glm::mat4 VP =camera->getProjectionMatrix(windowSize)*camera->getViewMatrix(); // projection * view
        //TODO: (Req 8) Set the OpenGL viewport using windowSize
        glViewport(0,0,windowSize[0],windowSize[1]); //  glViewport(0, 0, W/2, H/2);

        //TODO: (Req 8) Set the clear color to black and the clear depth to 1
        glClearColor(0.0f,0.0f,0.0f,0.0f); // clear the screen with a color of your choice.
        glClearDepth(1.0);  // set depth clear value (background = far plane). 
        //TODO: (Req 8) Set the color mask to true and the depth mask to true (to ensure the glClear will affect the framebuffer)
        glColorMask(true,true,true,true); // enable/disable writing colors in the frame buffer. (e.g. if cause red to stay red in all frames)
        glDepthMask(true);  // enables/disables writing on the depth buffer (enabled here.)

        // If there is a postprocess material, bind the framebuffer
        if(postprocessMaterial){
            //TODO: (Req 10) bind the framebuffer
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);
        }

        //TODO: (Req 8) Clear the color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //TODO: (Req 8) Draw all the opaque commands
        // Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        for(auto comm:opaqueCommands)
        {
            comm.material->setup();
            ShaderProgram * shader = comm.material->shader; // get the shader from current command
            glm::vec3 eye = glm::vec3(camera->getOwner()->getLocalToWorldMatrix()* glm::vec4 (0,0,0,1));
            shader->set("VP", VP); 
            shader->set("M", comm.localToWorld);
            shader->set("M_IT", glm::transpose(glm::inverse(comm.localToWorld)));
            shader->set("eye",eye);
            handleLights(world, shader);
            comm.mesh->draw();
        }
        // If there is a sky material, draw the sky
        if(this->skyMaterial){
            //TODO: (Req 9) setup the sky material
            skyMaterial->setup(); 
            //TODO: (Req 9) Get the camera position
            glm::vec3 cameraPosition = glm::vec3(camera->getOwner()->getLocalToWorldMatrix()* glm::vec4 (0,0,0,1));
            //TODO: (Req 9) Create a model matrix for the sky such that it always follows the camera (sky sphere center = camera position)
            // glm::mat4 identity = glm::mat4(
            //     //  Row1, Row2, Row3, Row4
            //     1.0f, 0.0f, 0.0f, 0.0f, // Column1
            //     0.0f, 1.0f, 0.0f, 0.0f, // Column2
            //     0.0f, 0.0f, 1.0f, 0.0f, // Column3
            //     0.0f, 0.0f, 0.0f, 1.0f  // Column4
            // );
            glm::mat4 skyModel = glm::translate(
               glm::mat4(1.0f) ,cameraPosition
            );
            //TODO: (Req 9) We want the sky to be drawn behind everything (in NDC space, z=1)
            // We can acheive the is by multiplying by an extra matrix after the projection but what values should we put in it?
            // newz = oldz * 0 + 1 = 1
            glm::mat4 alwaysBehindTransform = glm::mat4(
            //  Row1, Row2, Row3, Row4
                1.0f, 0.0f, 0.0f, 0.0f, // Column1
                0.0f, 1.0f, 0.0f, 0.0f, // Column2
                0.0f, 0.0f, 0.0f, 0.0f, // Column3
                0.0f, 0.0f, 1.0f, 1.0f  // Column4
            );

            //TODO: (Req 9) set the "transform" uniform
            ShaderProgram * shaderComm = skyMaterial->shader;
            shaderComm->set("transform",alwaysBehindTransform*VP*skyModel);
            //TODO: (Req 9) draw the sky sphere
            skySphere->draw();
        }
        //TODO: (Req 8) Draw all the transparent commands
        // Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        for(auto comm:transparentCommands)
        {
            comm.material->setup();
            ShaderProgram * shaderComm = comm.material->shader;// get the shader from current command
            shaderComm->set("transform",VP*comm.localToWorld);// set the transform matrix 
            comm.mesh->draw();
        }

        // If there is a postprocess material, apply postprocessing
        if(postprocessMaterial){
             //TODO: (Req 10) Return to the default framebuffer
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            //TODO: (Req 10) Setup the postprocess material and draw the fullscreen triangle
            std::cout<<"I am here "<<std::endl ; 
            postprocessMaterial->setup(); 
            postprocessMaterial->shader->set("inverse_projection",glm::inverse(camera->getProjectionMatrix(windowSize)));
            postprocessMaterial->shader->set("fog_color", fog.fog_color);
            postprocessMaterial->shader->set("fog_power", fog.fog_power);
            postprocessMaterial->shader->set("fog_exponent", 1.0f / fog.fog_distance);
                
            glBindVertexArray(postProcessVertexArray);
            glDrawArrays(GL_TRIANGLES,0,3); 
     
        }
    }

}