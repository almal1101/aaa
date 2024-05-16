#include "Server.h"

Server::Server(PlayerType ecPlayerType) : NetworkPlayer(ecPlayerType)
{}

void Server::Initialize(std::string sHost, int nPort, bool& bSwap)
{
    (void)sHost;

    bSwap = false;

    std::string sCommand;
    std::string sErrorMessage;
    std::string sToken;
    std::string sMessage;

    m_cLogger.UseLevelIndent(false);
    m_cLogger.UseTag(true);

    if (!Socket::Create())
        throw SocketException("Could not create server socket");

    m_cLogger.LogInfo("Socket created", 3);

    if (!Socket::Bind(nPort))
        throw SocketException("Could not bind to port");

    m_cLogger.LogInfo("Bind created", 3);

    if (!Socket::Listen())
        throw SocketException("Could not listen to socket");

    sMessage = "Server listening on port " + std::to_string(nPort);
    m_cLogger.LogInfo(sMessage, 2);

    std::cout << "Waiting for client to connect" << std::endl;

    if (!Socket::Accept())
        throw SocketException("Could not accept connection");

    m_cLogger.LogInfo("Accepted network connection", 3);

    if (Socket::Recv(sCommand) < 0)
        throw SocketException("Did not receive establish game command");

    sToken = GameVocabulary::ParseCommand(sCommand);
    if (sToken.compare(GameVocabulary::ESTABLISH_GAME) != 0)
    {
        sErrorMessage = "Expected command " + GameVocabulary::ESTABLISH_GAME + ", but received " + sCommand;
        std::cerr << sErrorMessage << std::endl;
        std::cout << "Exiting" << std::endl;
        throw GameAIException(sErrorMessage);
    }

    sToken = GameVocabulary::ParseArgument(sCommand);
    if (sToken.compare(GameTitle()) != 0)
    {
        sErrorMessage = "Server playing " + GameTitle() + " and client is playing " + sToken;
        std::cerr << sErrorMessage << std::endl;
        std::cout << "Exiting" << std::endl;
        Socket::Send(GameVocabulary::UNCONFIRM);
        exit(EXIT_FAILURE);
    }

    sMessage = "Confirming game of " + GameTitle() + " with client.";
    m_cLogger.LogInfo(sMessage, 2);

    Send(GameVocabulary::CONFIRM);

    if (Socket::Recv(sCommand) < 0)
        throw SocketException("Did not receive request for client player number");

    sToken = GameVocabulary::ParseCommand(sCommand);
    if (sToken.compare(GameVocabulary::REQUEST_CLIENT_PLAYER_NUMBER) != 0)
    {
        sErrorMessage = "Expected command " + GameVocabulary::REQUEST_CLIENT_PLAYER_NUMBER + ", but received " + sCommand;
        std::cerr << sErrorMessage << std::endl;
        std::cout << "Exiting" << std::endl;
        exit(EXIT_FAILURE);
    }

    sMessage = "Establishing client player number of " + std::to_string(m_nPlayerNumber);
    m_cLogger.LogInfo(sMessage, 2);

    sCommand = GameVocabulary::ESTABLISH_CLIENT_PLAYER_NUMBER + GameVocabulary::DELIMETER + std::to_string(m_nPlayerNumber);
    if (!Socket::Send(sCommand))
    {
        sErrorMessage = "Could not send command: " + sCommand;
        throw SocketException(sErrorMessage);
    }
}