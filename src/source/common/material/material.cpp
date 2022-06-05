#include "material.hpp"

#include "../asset-loader.hpp"
#include "deserialize-utils.hpp"

namespace our {

    // This function should setup the pipeline state and set the shader to be used
    void Material::setup() const {
        //TODO: (Req 6) Write this function
        pipelineState.setup();
        shader->use();
    }

    // This function read the material data from a json object
    void Material::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;

        if(data.contains("pipelineState")){
            pipelineState.deserialize(data["pipelineState"]);
        }
        shader = AssetLoader<ShaderProgram>::get(data["shader"].get<std::string>());
        transparent = data.value("transparent", false);
    }

    // This function should call the setup of its parent and
    // set the "tint" uniform to the value in the member variable tint 
    void TintedMaterial::setup() const {
        //TODO: (Req 6) Write this function
        Material::setup();
        shader->set("tint",tint);
    }

    // This function read the material data from a json object
    void TintedMaterial::deserialize(const nlohmann::json& data){
        Material::deserialize(data);
        if(!data.is_object()) return;
        tint = data.value("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // This function should call the setup of its parent and
    // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold
    // Then it should bind the texture and sampler to a texture unit and send the unit number to the uniform variable "tex" 
    void TexturedMaterial::setup() const {
        //TODO: (Req 6) Write this function
        TintedMaterial::setup();
        shader->set("alphaThreshold",alphaThreshold); // set the alphaThreshold value to the shader
        texture->bind(); // bind the texture 
        sampler->bind(0);// bind the sampler to unit #0
        shader->set("tex",0); //set the unit to be 0
    }

    // This function read the material data from a json object
    void TexturedMaterial::deserialize(const nlohmann::json& data){
        TintedMaterial::deserialize(data);
        if(!data.is_object()) return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }
     void LitMaterial::setup() const
    {
        Material::setup();

        //albedo
        glActiveTexture(GL_TEXTURE0);
        if (albedo)
            albedo->bind();
        else
            glBindTexture(GL_TEXTURE_2D,0);
        
        albedo_sampler->bind(0);
        shader->set("material.albedo", 0);
        

        //specular
        glActiveTexture(GL_TEXTURE1);
        if (specular)
            specular->bind();
        else
            glBindTexture(GL_TEXTURE_2D,0);
        specular_sampler->bind(1);
        shader->set("material.specular", 1);
        
        //ambient 
        glActiveTexture(GL_TEXTURE3);
        if (ambient_occlusion)
            ambient_occlusion->bind();
        else
            glBindTexture(GL_TEXTURE_2D,0);
        ambient_occlusion_sampler->bind(2);
        shader->set("material.ambient_occlusion", 2);

        //roughness 
        glActiveTexture(GL_TEXTURE2);
        if (roughness)
            roughness->bind();
        else
            glBindTexture(GL_TEXTURE_2D,0);
        roughness_sampler->bind(3);
        shader->set("material.roughness", 3);


        //emissive
        glActiveTexture(GL_TEXTURE4);
        if (emissive)
            emissive->bind();
        else
            glBindTexture(GL_TEXTURE_2D,0);
        emissive_sampler->bind(4);
        shader->set("material.emissive", 4);

        glActiveTexture(GL_TEXTURE0);
    }
    void LitMaterial::deserialize(const nlohmann::json &data)
    {
        Material::deserialize(data);
        if (!data.is_object())  return;

        if (data.contains("albedo"))
            albedo = AssetLoader<Texture2D>::get(data.value("albedo", ""));

        albedo_sampler= AssetLoader<Sampler>::get(data.value("albedo_sampler", "default"));

        if (data.contains("specular"))
            specular = AssetLoader<Texture2D>::get(data.value("specular", ""));
        
        specular_sampler = AssetLoader<Sampler>::get(data.value("specular_sampler", "default"));

        if (data.contains("ambient_occlusion"))
            ambient_occlusion = AssetLoader<Texture2D>::get(data.value("ambient_occlusion", ""));

        ambient_occlusion_sampler = AssetLoader<Sampler>::get(data.value("ambient_occlusion_sampler", "default"));

        if (data.contains("roughness"))
            roughness = AssetLoader<Texture2D>::get(data.value("roughness", ""));

        roughness_sampler = AssetLoader<Sampler>::get(data.value("roughness_sampler", "default"));


        if (data.contains("emission"))
            emissive = AssetLoader<Texture2D>::get(data.value("emissive", ""));

        emissive_sampler = AssetLoader<Sampler>::get(data.value("emissive_sampler", "default"));
    }


}