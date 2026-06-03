#pragma once
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include<iostream>
namespace YaoEngine
{
	class CSharpScriptEngine
	{
		public:
			CSharpScriptEngine()
			{
				mono_set_assemblies_path("D:\\dev\\YaoEngine DEV\\YaoEngine-DEV\\YaoEngine-core\\Dep\\Mono\\lib\\mono\\4.5");
				mono_set_dirs("D:\\dev\\YaoEngine DEV\\YaoEngine-DEV\\YaoEngine-core\\Dep\\Mono\\lib",
					"D:\\dev\\YaoEngine DEV\\YaoEngine-DEV\\YaoEngine-core\\Dep\\Mono\\etc");

				MonoDomain* domain = mono_jit_init("YaoJITRuntime");
				if (!domain) { 
					std::cerr << "Failed to initialize Mono JIT" << std::endl;
					return; }

				MonoAssembly* assembly = mono_domain_assembly_open(domain, "ScriptCore.dll");
				if (!assembly) { 
					std::cerr << "Failed to load assembly: ScriptCore.dll" << std::endl;
					return; }

				MonoImage* image = mono_assembly_get_image(assembly);
				if (!image) { 
					std::cerr << "image" << std::endl;
					return; }

				MonoClass* klass = mono_class_from_name(image, "Scriptcore", "demo");
				if (!klass) { 
					std::cerr << "klass" << std::endl;
					return; }

				MonoMethod* method = mono_class_get_method_from_name(klass, "Hello", 0);
				if (!method) { 
					std::cerr << "d" << std::endl;
					return; }

				mono_runtime_invoke(method, nullptr, nullptr, nullptr);
			}

	};


	class PythonScriptEngine
	{
	};
	
}