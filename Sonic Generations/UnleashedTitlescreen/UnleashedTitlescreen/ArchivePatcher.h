#pragma once


struct ArchiveDependency
{
    std::string archive;
    std::vector<std::string> dependencies;

    ArchiveDependency() {};
    ArchiveDependency(std::string archive, std::vector<std::string> dependencies) : archive(archive), dependencies(dependencies) {};
};



class ArchivePatcher
{
public:
    static std::vector<ArchiveDependency> archiveDependencies;

    static void Install();
};