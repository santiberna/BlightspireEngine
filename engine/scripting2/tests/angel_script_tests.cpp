#include <gtest/gtest.h>

#include <angelscript.h>
#include <new>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptstdstring/scriptstdstring.h>
#include <spdlog/spdlog.h>

constexpr const char* SCRIPT =
#include "script.as"
    ;

// Implement a simple message callback function
void messageCallback(const asSMessageInfo* msg, void* param)
{
    switch (msg->type)
    {
    case asMSGTYPE_ERROR:
        spdlog::error("{0} (Row {1}, Col {2}): {3}", msg->section, msg->row, msg->col, msg->message);
        break;
    case asMSGTYPE_WARNING:
        spdlog::warn("{0} (Row {1}, Col {2}): {3}", msg->section, msg->row, msg->col, msg->message);
        break;
    case asMSGTYPE_INFORMATION:
        spdlog::info("{0} (Row {1}, Col {2}): {3}", msg->section, msg->row, msg->col, msg->message);
        break;
    }
}

void print(const std::string& in)
{
    GTEST_LOG_(INFO) << in;
}

struct Player
{
    int health = 100;
    std::string name = "Santi";

    void sayName()
    {
        print(name);
    }
};

template <typename T>
    requires std::is_default_constructible_v<T>
void Constructor(void* memory)
{
    // Initialize the pre-allocated memory by calling the
    // object constructor with the placement-new operator
    new (memory) T();
}

template <typename T>
void Destructor(void* memory)
{
    // Uninitialize the memory by calling the object destructor
    ((T*)memory)->~T();
}

TEST(AngelScriptTests, HelloWorld)
{
    // Create the script engine
    asIScriptEngine* engine = asCreateScriptEngine();

    // Set the message callback to receive information on errors in human readable form.
    int r = engine->SetMessageCallback(asFUNCTION(messageCallback), nullptr, asCALL_CDECL);
    assert(r >= 0);

    // AngelScript doesn't have a built-in string type, as there is no definite standard
    // string type for C++ applications. Every developer is free to register its own string type.
    // The SDK do however provide a standard add-on for registering a string type, so it's not
    // necessary to implement the registration yourself if you don't want to.
    RegisterStdString(engine);

    // Register the function that we want the scripts to call
    r = engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL);
    assert(r >= 0);

    r = engine->RegisterObjectType("Player", sizeof(Player), asOBJ_VALUE | asGetTypeTraits<Player>());
    assert(r >= 0);

    r = engine->RegisterObjectMethod("Player", "void sayName()", asMETHOD(Player, sayName), asCALL_THISCALL);

    r = engine->RegisterObjectBehaviour("Player", asBEHAVE_CONSTRUCT, "void construct()", asFUNCTION(Constructor<Player>), asCALL_CDECL_OBJLAST);
    assert(r >= 0);

    r = engine->RegisterObjectBehaviour("Player", asBEHAVE_DESTRUCT, "void destroy()", asFUNCTION(Destructor<Player>), asCALL_CDECL_OBJLAST);
    assert(r >= 0);
    // The CScriptBuilder helper is an add-on that loads the file,
    // performs a pre-processing pass if necessary, and then tells
    // the engine to build a script module.
    CScriptBuilder builder;
    r = builder.StartNewModule(engine, "MyModule");
    if (r < 0)
    {
        // If the code fails here it is usually because there
        // is no more memory to allocate the module
        printf("Unrecoverable error while starting a new module.\n");
        FAIL();
    }
    r = builder.AddSectionFromMemory("test",
        SCRIPT);

    if (r < 0)
    {
        // The builder wasn't able to load the file. Maybe the file
        // has been removed, or the wrong name was given, or some
        // preprocessing commands are incorrectly written.
        printf("Please correct the errors in the script and try again.\n");
        FAIL();
    }
    r = builder.BuildModule();
    if (r < 0)
    {
        // An error occurred. Instruct the script writer to fix the
        // compilation errors that were listed in the output stream.
        printf("Please correct the errors in the script and try again.\n");
        FAIL();
    }

    // Find the function that is to be called.
    asIScriptModule* mod = engine->GetModule("MyModule");
    asIScriptFunction* func = mod->GetFunctionByDecl("void main()");
    if (func == nullptr)
    {
        // The function couldn't be found. Instruct the script writer
        // to include the expected function in the script.
        printf("The script must have the function 'void main()'. Please add it and try again.\n");
        FAIL();
    }

    // Create our context, prepare it, and then execute
    asIScriptContext* ctx = engine->CreateContext();
    ctx->Prepare(func);
    r = ctx->Execute();
    if (r != asEXECUTION_FINISHED)
    {
        // The execution didn't complete as expected. Determine what happened.
        if (r == asEXECUTION_EXCEPTION)
        {
            // An exception occurred, let the script writer know what happened so it can be corrected.
            printf("An exception '%s' occurred. Please correct the code and try again.\n", ctx->GetExceptionString());
        }
    }

    // Clean up
    ctx->Release();
    engine->ShutDownAndRelease();
}
