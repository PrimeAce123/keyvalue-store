#define CATCH_CONFIG_RUNNER

#include "catch/catch.hpp"

int main(int argc_in, char **argv_in) {
    std::cout << "Setting up before running tests..." << std::endl;

    Catch::Session session;

    int returnCode = session.applyCommandLine(argc_in, argv_in);
    if (returnCode != 0) {
        std::cout << "Invalid command line arguments" << std::endl;
        return returnCode;
    }

    /*
     * You can tweak any config here if you want
     * session.configData().showSuccessfulTests = true;
     */
    int numFailed = session.run();

    std::cout << "[Cleanup] Done running tests.\n";

    return numFailed;
}