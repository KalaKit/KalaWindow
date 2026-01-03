//------------------------------------------------------------------------------
// file_utils.hpp
//
// Copyright (C) 2026 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
//
// Provides:
//   - file management - create file, create directory, list directory contents, rename, delete, copy, move
//   - file metadata - file size, directory size, line count, set extension
//   - text I/O - read/write data for text files with vector of string lines or string blob
//   - binary I/O - read/write data for binary files with vector of bytes or buffer + size
//------------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <cerrno>
#include <cstring>

//reinterpret_cast
#ifndef rcast
	#define rcast reinterpret_cast
#endif

//static_cast
#ifndef scast
	#define scast static_cast
#endif

namespace KalaHeaders::KalaFile
{	
	constexpr size_t TEN_MB = 10ULL * 1024 * 1024;
	constexpr size_t ONE_GB = 1ULL * 1024 * 1024 * 1024;
	constexpr size_t CHUNK_64KB = 64ULL * 1024;
	constexpr size_t CHUNK_1MB = 1ULL * 1024 * 1024;

	using std::exception;
	using std::string;
	using std::vector;
	using std::ostringstream;
	using std::istreambuf_iterator;
	using std::ifstream;
	using std::ofstream;
	using std::streamsize;
	using std::streamoff;
	using std::ios;
	using std::search;
	using std::distance;
	using std::min;
	using std::filesystem::exists;
	using std::filesystem::path;
	using std::filesystem::is_regular_file;
	using std::filesystem::is_directory;
	using std::filesystem::copy_file;
	using std::filesystem::copy_options;
	using std::filesystem::copy;
	using std::filesystem::rename;
	using std::filesystem::remove;
	using std::filesystem::remove_all;
	using std::filesystem::create_directories;
	using std::filesystem::file_size;
	using std::filesystem::recursive_directory_iterator;
	using std::filesystem::directory_iterator;
	using std::filesystem::status;
	using std::filesystem::perms;
	using std::strerror;
	
	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using i8 = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;

	enum class FileType
	{
		FILE_TEXT,
		FILE_BINARY
	};

	//Data struct that is used for creating a new file.
	//One of the four data blocks must also be filled (inBuffer + bufferSize are together)
	struct FileData
	{
		//Vector of bytes that will be written into new file
		vector<uint8_t> inData{};

		//String blob that will be written into new file
		string inText{};

		//Vector of strings that will be written into new file
		vector<string> inLines{};
	};

	//Start and end of chosen string or bytes value in a binary file
	struct BinaryRange
	{
		size_t start{};
		size_t end{};
	};

	//
	// FILE MANAGEMENT
	//

	//Forward declarations for write* functions

	inline string WriteTextToFile(
		const path& target,
		const string& inText,
		bool append = false);

	inline string WriteLinesToFile(
		const path& target,
		const vector<string>& inLines,
		bool append = false);

	inline string WriteBinaryLinesToFile(
		const path& target,
		const vector<uint8_t>& inData,
		bool append = false);

	//Create regular or binary file at target path. If you also want data written
	//to the new file after its been created then pass a fileData struct
	//with one of the fields filled in, only the first found field data is used
	inline string CreateFile(
		const path& target,
		FileType targetFileType = FileType::FILE_BINARY,
		const FileData& fileData = FileData{})
	{
		ostringstream oss{};

		if (target.empty())
		{
			oss << "Failed to create new file because no target path was passed!";

			return oss.str();
		}
		if (exists(target))
		{
			oss << "Failed to create new file at path '" << target
				<< "' because it already exists!";

			return oss.str();
		}
		
		auto fileStatus = status(target.parent_path());
		auto filePerms = fileStatus.permissions();
		
		bool canWrite = (filePerms & (
			perms::owner_write
			| perms::group_write
			| perms::others_write))  
			!= perms::none;
		
		if (!canWrite)
		{
			oss << "Failed to create target '" << target << "' because of insufficient write permissions!";

			return oss.str();
		}

		switch (targetFileType)
		{
		case FileType::FILE_TEXT:
		{
			if (!fileData.inData.empty())
			{
				oss << "Failed to create new file at path '" << target
					<< "' because its type was set to 'FILE_TEXT' and binary data was passed to it!";

				return oss.str();
			}

			ofstream file(
				target,
				ios::out
				| ios::trunc);

			file.close();

			if (!fileData.inLines.empty()
				|| !fileData.inText.empty())
			{
				string result{};

				if (!fileData.inText.empty())
				{
					result = WriteTextToFile(
						target,
						fileData.inText);
				}
				else if (!fileData.inLines.empty())
				{
					result = WriteLinesToFile(
						target,
						fileData.inLines);
				}

				if (!result.empty())
				{
					oss << "Failed to create new text file '"
						<< target << "'! Reason: " << result;

					return oss.str();
				}
			}
			break;
		}
		case FileType::FILE_BINARY:
		{
			if (!fileData.inLines.empty()
				|| !fileData.inText.empty())
			{
				oss << "Failed to create new file at path '" << target
					<< "' because its type was set to 'FILE_BINARY' and string data was passed to it!";

				return oss.str();
			}

			ofstream file(
				target,
				ios::out
				| ios::binary
				| ios::trunc);

			file.close();

			if (!fileData.inData.empty())
			{
				string result{};

				result = WriteBinaryLinesToFile(
					target,
					fileData.inData);

				if (!result.empty())
				{
					oss << "Failed to create new binary file '"
						<< target << "'! Reason: " << result;

					return oss.str();
				}
			}
			break;
		}
		}

		return{};
	}

	//Create a directory at target path, this also creates all
	//parent folders up to it that don't exist yet
	inline string CreateDirectory(const path& target)
	{
		ostringstream oss{};

		if (exists(target))
		{
			oss << "Failed to create target '" << target << "' because it already exists!";

			return oss.str();
		}
		if (target.has_extension())
		{
			oss << "Failed to create target '" << target << "' because it has an extension!";

			return oss.str();
		}
		
		auto fileStatus = status(target.parent_path());
		auto filePerms = fileStatus.permissions();
		
		bool canWrite = (filePerms & (
			perms::owner_write
			| perms::group_write
			| perms::others_write))  
			!= perms::none;
		
		if (!canWrite)
		{
			oss << "Failed to create directory '" << target << "' because of insufficient write permissions!";

			return oss.str();
		}

		try
		{
			create_directories(target);
		}
		catch (exception& e)
		{
			oss << "Failed to create target '" << target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}

	//List all the contents of a folder, with optional recursive flag
	inline string ListDirectoryContents(
		const path& target,
		vector<path>& outEntries,
		bool recursive = false)
	{
		ostringstream oss{};

		if (!exists(target))
		{
			oss << "Failed to list paths from target '" << target << "' because it does not exist!";

			return oss.str();
		}
		if (!is_directory(target))
		{
			oss << "Failed to list paths from target '" << target << "' because it is not a directory!";

			return oss.str();
		}
		
		auto fileStatus = status(target);
		auto filePerms = fileStatus.permissions();
		
		bool canRead = (filePerms & (
			perms::owner_read
			| perms::group_read
			| perms::others_read))  
			!= perms::none;
		
		if (!canRead)
		{
			oss << "Failed to list directory contents from target '" << target << "' because of insufficient read permissions!";

			return oss.str();
		}

		try
		{
			if (recursive)
			{
				for (auto& entry : recursive_directory_iterator(target))
				{
					outEntries.push_back(entry.path());
				}
			}
			else
			{
				for (auto& entry : directory_iterator(target))
				{
					outEntries.push_back(entry.path());
				}
			}
		}
		catch (exception& e)
		{
			oss << "Failed to list paths from target '" << target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}

	//Rename file or folder in its current directory
	inline string RenamePath(
		const path& target,
		const string& newName)
	{
		ostringstream oss{};

		if (!exists(target))
		{
			oss << "Failed to rename target '" << target << "' to '"
				<< newName << "' because it does not exist!";

			return oss.str();
		}
		if (is_directory(target)
			&& path(newName).has_extension())
		{
			oss << "Failed to rename target '" << target << "' to '"
				<< newName << "' because target is a directory but new name has an extension!";

			return oss.str();
		}
		if (is_regular_file(target)
			&& newName.empty())
		{
			oss << "Failed to rename target '" << target << "' to '"
				<< newName << "' because target is a file but new name is empty!";

			return oss.str();
		}
		
		auto fileStatus = status(target.parent_path());
		auto filePerms = fileStatus.permissions();
		
		bool canWrite = (filePerms & (
			perms::owner_write
			| perms::group_write
			| perms::others_write))  
			!= perms::none;

		if (!canWrite)
		{
			oss << "Failed to rename target '" << target << "' because of insufficient write permissions!";

			return oss.str();
		}

		try
		{
			path newTarget = target.parent_path() / newName;
			rename(target, newTarget);
		}
		catch (exception& e)
		{
			oss << "Failed to rename '"
				<< target << "' to '"
				<< newName << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}

	//Delete file or folder in target path (recursive for directories)
	inline string DeletePath(const path& target)
	{
		ostringstream oss{};

		if (!exists(target))
		{
			oss << "Failed to delete target '"
				<< target << "' because it does not exist!";

			return oss.str();
		}
		
		auto fileStatus = status(target.parent_path());
		auto filePerms = fileStatus.permissions();
		
		bool canWrite = (filePerms & (
			perms::owner_write
			| perms::group_write
			| perms::others_write))  
			!= perms::none;

		if (!canWrite)
		{
			oss << "Failed to delete target '" << target << "' because of insufficient write permissions!";

			return oss.str();
		}

		try
		{
			if (is_regular_file(target)) remove(target);
			else if (is_directory(target)) remove_all(target);
		}
		catch (exception& e)
		{
			oss << "Failed to delete '"
				<< target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}

	//Copy file or folder from origin to target, with optional overwrite flag
	inline string CopyPath(
		const path& origin,
		const path& target,
		bool overwrite = false)
	{
		ostringstream oss{};

		if (!exists(origin))
		{
			oss << "Failed to copy origin to target because origin '"
				<< origin << "' does not exist!";

			return oss.str();
		}
		if (exists(target)
			&& overwrite)
		{
			string result = DeletePath(target);
			if (!result.empty())
			{
				oss << "Failed to copy origin '"
					<< origin << "' to target '"
					<< target << "' because overwrite was enabled and target couldn't be deleted! Reason: " << result;

				return oss.str();
			}
		}
		if (is_directory(origin)
			&& target.has_extension())
		{
			oss << "Failed to copy origin '" << origin << "' to '"
				<< target << "' because origin is a directory but target has an extension!";

			return oss.str();
		}
		if (is_regular_file(origin)
			&& target.empty())
		{
			oss << "Failed to copy origin '" << origin << "' to '"
				<< target << "' because origin is a file but target is empty!";

			return oss.str();
		}
		
		auto fileStatusOrigin = status(origin.parent_path());
		auto filePermsOrigin = fileStatusOrigin.permissions();
		
		bool canWriteOrigin = (filePermsOrigin & (
			perms::owner_write
			| perms::group_write
			| perms::others_write))  
			!= perms::none;

		if (!canWriteOrigin)
		{
			oss << "Failed to copy origin '" << origin << "' to target '" << target << "' because of insufficient write permissions for origin!";

			return oss.str();
		}
		
		auto fileStatusTarget = status(target.parent_path());
		auto filePermsTarget = fileStatusTarget.permissions();
		
		bool canWriteTarget = (filePermsTarget & (
			perms::owner_write
			| perms::group_write
			| perms::others_write))  
			!= perms::none;

		if (!canWriteTarget)
		{
			oss << "Failed to copy origin '" << origin << "' to target '" << target << "' because of insufficient write permissions for target!";

			return oss.str();
		}

		try
		{
			if (is_regular_file(origin))
			{
				copy_file(
					origin,
					target,
					overwrite
					? copy_options::overwrite_existing
					: copy_options::skip_existing);
			}
			else if (is_directory(origin))
			{
				copy(
					origin,
					target,
					overwrite
					? copy_options::overwrite_existing
					| copy_options::recursive
					: copy_options::skip_existing
					| copy_options::recursive);
			}
		}
		catch (exception& e)
		{
			oss << "Failed to copy '"
				<< origin << "' to target '"
				<< target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}

	//Move file or folder from origin to target, target is always overwritten if it already exists
	inline string MovePath(
		const path& origin,
		const path& target)
	{
		ostringstream oss{};

		if (!exists(origin))
		{
			oss << "Failed to move origin to target because origin '"
				<< origin << "' does not exist!";

			return oss.str();
		}
		if (exists(target))
		{
			string result = DeletePath(target);
			if (!result.empty())
			{
				oss << "Failed to move origin '"
					<< origin << "' to target '"
					<< target << "' because it existed and it couldn't be deleted! Reason: " << result;

				return oss.str();
			}
		}
		if (is_directory(origin)
			&& target.has_extension())
		{
			oss << "Failed to move origin '" << origin << "' to '"
				<< target << "' because origin is a directory but target has an extension!";

			return oss.str();
		}
		if (is_regular_file(origin)
			&& target.empty())
		{
			oss << "Failed to move origin '" << origin << "' to '"
				<< target << "' because origin is a file but target is empty!";

			return oss.str();
		}
		
		auto fileStatusOrigin = status(origin.parent_path());
		auto filePermsOrigin = fileStatusOrigin.permissions();
		
		bool canWriteOrigin = (filePermsOrigin & (
			perms::owner_write
			| perms::group_write
			| perms::others_write))  
			!= perms::none;

		if (!canWriteOrigin)
		{
			oss << "Failed to move origin '" << origin << "' to target '" << target << "' because of insufficient write permissions for origin!";

			return oss.str();
		}
		
		auto fileStatusTarget = status(target.parent_path());
		auto filePermsTarget = fileStatusTarget.permissions();
		
		bool canWriteTarget = (filePermsTarget & (
			perms::owner_write
			| perms::group_write
			| perms::others_write))  
			!= perms::none;

		if (!canWriteTarget)
		{
			oss << "Failed to move origin '" << origin << "' to target '" << target << "' because of insufficient write permissions for target!";

			return oss.str();
		}

		try
		{
			rename(origin, target);
		}
		catch (exception& e)
		{
			oss << "Failed to move '"
				<< origin << "' to target '"
				<< target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}

	//
	// FILE METADATA
	//

	//Get the size of the target file in bytes
	inline string GetFileSize(
		const path& target,
		uintmax_t& outSize)
	{
		ostringstream oss{};

		if (!exists(target))
		{
			oss << "Failed to get target file '" << target << "' size because it does not exist!";

			return oss.str();
		}
		if (!is_regular_file(target))
		{
			oss << "Failed to get target file '" << target << "' size because it is not a regular file!";

			return oss.str();
		}
		
		auto fileStatus = status(target.parent_path());
		auto filePerms = fileStatus.permissions();
		
		bool canRead = (filePerms & (
			perms::owner_read
			| perms::group_read
			| perms::others_read))  
			!= perms::none;
		
		if (!canRead)
		{
			oss << "Failed to get file size from target '" << target << "' because of insufficient read permissions!";

			return oss.str();
		}

		try
		{
			outSize = file_size(target);
		}
		catch (exception& e)
		{
			oss << "Failed to get target file '" << target << "' size! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}

	//Get the size of the target directory in bytes
	inline string GetDirectorySize(
		const path& target,
		uintmax_t& outSize)
	{
		ostringstream oss{};
		uintmax_t totalSize{};

		if (!exists(target))
		{
			oss << "Failed to get target directory '" << target << "' size because it does not exist!";

			return oss.str();
		}
		if (!is_directory(target))
		{
			oss << "Failed to get target directory '" << target << "' size because it is not a directory!";

			return oss.str();
		}
		
		auto fileStatus = status(target);
		auto filePerms = fileStatus.permissions();
		
		bool canRead = (filePerms & (
			perms::owner_read
			| perms::group_read
			| perms::others_read))  
			!= perms::none;
		
		if (!canRead)
		{
			oss << "Failed to get directory size from target '" << target << "' because of insufficient read permissions!";

			return oss.str();
		}

		try
		{
			for (const auto& file : recursive_directory_iterator(target))
			{
				//skip directories
				if (is_directory(file)) continue;

				uintmax_t fileSize{};
				string result = GetFileSize(file, fileSize);

				if (!result.empty())
				{
					oss << "Failed to get target directory '"
						<< target << "' size because one of its file sizes couldn't be read! Reason: '" << result;

					return oss.str();
				}

				totalSize += fileSize;
			}
			outSize = totalSize;
		}
		catch (exception& e)
		{
			oss << "Failed to get target directory '" << target << "' size! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}

	//Get the count of lines in a text file
	inline string GetTextFileLineCount(
		const path& target,
		size_t& outCount)
	{
		ostringstream oss{};
		size_t totalCount{};

		if (!exists(target))
		{
			oss << "Failed to get target '" << target << "' line count because it does not exist!";

			return oss.str();
		}
		if (!is_regular_file(target))
		{
			oss << "Failed to get target '" << target << "' line count because it is not a regular file!";

			return oss.str();
		}
		
		auto fileStatus = status(target);
		auto filePerms = fileStatus.permissions();
		
		bool canRead = (filePerms & (
			perms::owner_read
			| perms::group_read
			| perms::others_read))  
			!= perms::none;
		
		if (!canRead)
		{
			oss << "Failed to get text file line count from target '" << target << "' because of insufficient read permissions!";

			return oss.str();
		}

		try
		{
			ifstream in(
				target, 
				ios::in);

			if (in.fail()
				&& errno != 0)
			{
				int err = errno;

				oss << "Failed to get target '" << target
					<< "' line count because it couldn't be opened! "
					<< "Reason: (errno " << err << "): " << strerror(err);

				return oss.str();
			}

			string tmp{};
			while (getline(in, tmp)) ++totalCount;

			if (totalCount == 0)
			{
				oss << "Failed to get target '" << target << "' line count because it had no lines!";

				return oss.str();
			}

			outCount = totalCount;

			in.close();
		}
		catch (exception& e)
		{
			oss << "Failed to get target '" << target << "' line count! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}

	//Set the extension of the target
	inline string SetPathExtension(
		const path& target,
		const string& newExtension,
		string& outResult)
	{
		ostringstream oss{};

		if (!exists(target))
		{
			oss << "Failed to set target '"
				<< target << "' extension because it does not exist!";

			return oss.str();
		}
		if (!is_regular_file(target))
		{
			oss << "Failed to set extension for target '"
				<< target << "' because it is not a regular file!";

			return oss.str();
		}
		
		auto fileStatus = status(target);
		auto filePerms = fileStatus.permissions();
		
		bool canWrite = (filePerms & (
			perms::owner_write
			| perms::group_write
			| perms::others_write))  
			!= perms::none;
		
		if (!canWrite)
		{
			oss << "Failed to set path extension for target '" << target << "' because of insufficient write permissions!";

			return oss.str();
		}

		try
		{
			path newTarget = target;
			newTarget.replace_extension(newExtension);

			string result = RenamePath(
				target,
				newTarget.filename().string());

			if (!result.empty())
			{
				oss << "Failed to set target extension to '"
					<< newExtension << "' because RenameTarget failed! Reason: " << result;

				return oss.str();
			}

			outResult = newTarget.string();
		}
		catch (exception& e)
		{
			oss << "Failed to set extension for target '"
				<< target << "' to '" + newExtension + "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}

	//
	// TEXT I/O
	//

	//Write all text from a string to a text file, with optional append flag.
	//A new file is created at target path if it doesn't already exist
	inline string WriteTextToFile(
		const path& target,
		const string& inText,
		bool append)
	{
		ostringstream oss{};

		if (exists(target)
			&& !is_regular_file(target))
		{
			oss << "Failed to write text to target '" << target << "' because it is not a regular file!";

			return oss.str();
		}
		if (inText.empty())
		{
			oss << "Failed to write text to target '" << target << "' because inText string is empty!";

			return oss.str();
		}
		
		auto fileStatus = status(target);
		auto filePerms = fileStatus.permissions();
		
		bool canWrite = (filePerms & (
			perms::owner_write
			| perms::group_write
			| perms::others_write))  
			!= perms::none;
		
		if (!canWrite)
		{
			oss << "Failed to write text to target '" << target << "' because of insufficient write permissions!";

			return oss.str();
		}

		try
		{
			ofstream out;

			if (append)
			{
				out.open(
					target,
					ios::out
					| ios::app);
			}
			else
			{
				out.open(
					target,
					ios::out
					| ios::trunc);
			}

			if (out.fail()
				&& errno != 0)
			{
				int err = errno;

				oss << "Failed to write text to target '" << target
					<< "' because it couldn't be opened! "
					<< "Reason: (errno " << err << "): " << strerror(err);

				return oss.str();
			}

			out << inText;

			out.close();
		}
		catch (exception& e)
		{
			oss << "Failed to write text to target '" << target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}
	//Read all text from a file into a string
	inline string ReadTextFromFile(
		const path& target, 
		string& outText)
	{
		ostringstream oss{};
		string allText{};

		if (!exists(target))
		{
			oss << "Failed to read text from target '" << target << "' because it does not exist!";

			return oss.str();
		}
		if (!is_regular_file(target))
		{
			oss << "Failed to read text from target '" << target << "' because it is not a regular file!";

			return oss.str();
		}
		
		auto fileStatus = status(target);
		auto filePerms = fileStatus.permissions();
		
		bool canRead = (filePerms & (
			perms::owner_read
			| perms::group_read
			| perms::others_read))  
			!= perms::none;
		
		if (!canRead)
		{
			oss << "Failed to get text from target '" << target << "' because of insufficient read permissions!";

			return oss.str();
		}

		try
		{
			ifstream in(
				target,
				ios::in);

			if (in.fail()
				&& errno != 0)
			{
				int err = errno;

				oss << "Failed to read text from target '" << target
					<< "' because it couldn't be opened! "
					<< "Reason: (errno " << err << "): " << strerror(err);

				return oss.str();
			}

			ostringstream buffer{};

			buffer << in.rdbuf();
			allText = buffer.str();

			in.close();

			if (allText.empty())
			{
				oss << "Failed to read text from target '" << target << "' because it was empty!";

				return oss.str();
			}

			//successfully got data
			outText = allText;
		}
		catch (exception& e)
		{
			oss << "Failed to read text from target '" << target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}

	//Write all lines from a vector to a text file, with optional append flag.
	//A new file is created at target path if it doesn't already exist
	inline string WriteLinesToFile(
		const path& target,
		const vector<string>& inLines,
		bool append)
	{
		ostringstream oss{};

		if (exists(target)
			&& !is_regular_file(target))
		{
			oss << "Failed to write lines to target '" << target << "' because it is not a regular file!";

			return oss.str();
		}
		if (inLines.empty())
		{
			oss << "Failed to write lines to target '" << target << "' because inLines vector is empty!";

			return oss.str();
		}
		
		auto fileStatus = status(target);
		auto filePerms = fileStatus.permissions();
		
		bool canWrite = (filePerms & (
			perms::owner_write
			| perms::group_write
			| perms::others_write))  
			!= perms::none;
		
		if (!canWrite)
		{
			oss << "Failed to write lines to target '" << target << "' because of insufficient write permissions!";

			return oss.str();
		}

		try
		{
			ofstream out;

			if (append)
			{
				out.open(
					target,
					ios::out
					| ios::app);
			}
			else
			{
				out.open(
					target,
					ios::out
					| ios::trunc);
			}

			if (out.fail()
				&& errno != 0)
			{
				int err = errno;

				oss << "Failed to write lines to target '" << target
					<< "' because it couldn't be opened! "
					<< "Reason: (errno " << err << "): " << strerror(err);

				return oss.str();
			}

			for (size_t i = 0; i < inLines.size(); ++i)
			{
				out << inLines[i] << "\n";
			}

			out.close();
		}
		catch (exception& e)
		{
			oss << "Failed to write lines to target '" << target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}
	//Read all lines from a file into a vector of strings with optional 
	//lineStart and lineEnd values to avoid placing all lines to memory.
	//If lineEnd is 0 and lineStart isnt, then this function defaults end to EOF
	inline string ReadLinesFromFile(
		const path& target,
		vector<string>& outLines,
		size_t lineStart = 0,
		size_t lineEnd = 0)
	{
		ostringstream oss{};
		vector<string> allLines{};

		if (!exists(target))
		{
			oss << "Failed to read lines from target '" << target << "' because it does not exist!";

			return oss.str();
		}
		if (!is_regular_file(target))
		{
			oss << "Failed to read lines from target '" << target << "' because it is not a regular file!";

			return oss.str();
		}
		
		auto fileStatus = status(target);
		auto filePerms = fileStatus.permissions();
		
		bool canRead = (filePerms & (
			perms::owner_read
			| perms::group_read
			| perms::others_read))  
			!= perms::none;
		
		if (!canRead)
		{
			oss << "Failed to read lines from target '" << target << "' because of insufficient read permissions!";

			return oss.str();
		}

		try
		{
			ifstream in(
				target,
				ios::in);

			if (in.fail()
				&& errno != 0)
			{
				int err = errno;

				oss << "Failed to read lines from target '" << target
					<< "' because it couldn't be opened! "
					<< "Reason: (errno " << err << "): " << strerror(err);

				return oss.str();
			}

			size_t totalLines{};
			string result = GetTextFileLineCount(target, totalLines);

			if (!result.empty())
			{
				oss << "Failed to read lines from target '"
					<< target << "'! Reason: '" << result;

				return oss.str();
			}

			if (lineEnd == 0) lineEnd = totalLines;

			if (lineEnd <= lineStart)
			{
				oss << "Failed to read lines from target '"
					<< target << "' because lineEnd '"
					<< lineEnd << "' is lower or equal to lineStart '" << lineStart << "'!";

				return oss.str();
			}
			if (lineStart >= lineEnd)
			{
				oss << "Failed to read lines from target '"
					<< target << "' because lineStart '"
					<< lineStart << "' is higher or equal to lineEnd '" << lineEnd << "'!";

				return oss.str();
			}
			if (lineStart >= totalLines)
			{
				oss << "Failed to read lines from target '"
					<< target << "' because lineStart '"
					<< lineStart << "' is higher or equal to totalLines '" << totalLines << "'!";

				return oss.str();
			}
			if (lineEnd > totalLines)
			{
				oss << "Failed to read lines from target '"
					<< target << "' because lineEnd '"
					<< lineEnd << "' is higher than totalLines '" << totalLines << "'!";

				return oss.str();
			}

			string line{};
			size_t currentLine{};
			while (getline(in, line))
			{
				if (currentLine >= lineStart
					&& currentLine < lineEnd)
				{
					allLines.push_back(line);
				}
				++currentLine;

				if (currentLine >= lineEnd) break;
			}

			in.close();

			size_t expected = lineEnd - lineStart;
			if (allLines.size() != expected)
			{
				oss << "Failed to read lines from target '" << target << "'!"
					<< " Expected size was '" << expected << "' lines but result was '" << allLines.size() << "' lines.";

				return oss.str();
			}

			//successfully got data
			outLines = move(allLines);
		}
		catch (exception& e)
		{
			oss << "Failed to read lines from target '" << target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}

	//
	// BINARY I/O
	//

	//Simple helper to get binary chunk stream size for efficient binary reading
	inline size_t GetBinaryChunkStreamSize(size_t fileSize)
	{
		//empty file
		if (fileSize == 0) return 0;

		//whole file is under 10MB, use as single chunk
		if (fileSize < TEN_MB) return fileSize;

		//whole file is 10MB to 1GB - return 64KB chunk size
		if (fileSize < ONE_GB) return CHUNK_64KB;

		//whole file size is over 1GB - return 1MB chunk size
		return CHUNK_1MB;
	}

	//Write all binary data from a vector<uint8_t> to a file, with optional append flag.
	//A new file is created at target path if it doesn't already exist
	inline string WriteBinaryLinesToFile(
		const path& target,
		const vector<uint8_t>& inData,
		bool append)
	{
		ostringstream oss{};

		if (exists(target)
			&& !is_regular_file(target))
		{
			oss << "Failed to write binary to target '" << target << "' because it is not a regular file!";

			return oss.str();
		}
		if (inData.empty())
		{
			oss << "Failed to write binary to target '" << target << "' because inData vector is empty!";

			return oss.str();
		}
		
		auto fileStatus = status(target);
		auto filePerms = fileStatus.permissions();
		
		bool canWrite = (filePerms & (
			perms::owner_write
			| perms::group_write
			| perms::others_write))  
			!= perms::none;
		
		if (!canWrite)
		{
			oss << "Failed to write binary lines to target '" << target << "' because of insufficient write permissions!";

			return oss.str();
		}

		try
		{
			ofstream out;

			if (append)
			{
				out.open(
					target,
					ios::out
					| ios::binary
					| ios::app);
			}
			else
			{
				out.open(
					target,
					ios::out
					| ios::binary
					| ios::trunc);
			}

			if (out.fail()
				&& errno != 0)
			{
				int err = errno;

				oss << "Failed to write binary lines to target '" << target
					<< "' because it couldn't be opened! "
					<< "Reason: (errno " << err << "): " << strerror(err);

				return oss.str();
			}

			out.write(
				rcast<const char*>(inData.data()),
				scast<streamsize>(inData.size()));

			if (out.fail()
				&& errno != 0)
			{
				int err = errno;

				oss << "Failed to write binary lines to target '" << target
					<< "' because it couldn't be opened! "
					<< "Reason: (errno " << err << "): " << strerror(err);

				return oss.str();
			}

			out.close();
		}
		catch (exception& e)
		{
			oss << "Failed to write binary lines to target '" << target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}
	//Read all binary data from a file into a vector<uint8_t> with optional 
	//rangeStart and rangeEnd values to avoid placing whole binary file to memory.
	//If rangeEnd is 0 and rangeStart isnt, then this function defaults end to EOF
	inline string ReadBinaryLinesFromFile(
		const path& target,
		vector<uint8_t>& outData,
		size_t rangeStart = 0,
		size_t rangeEnd = 0)
	{
		ostringstream oss{};
		vector<uint8_t> allData{};

		if (!exists(target))
		{
			oss << "Failed to read binary from target '" << target << "' because it does not exist!";

			return oss.str();
		}
		if (!is_regular_file(target))
		{
			oss << "Failed to read binary from target '" << target << "' because it is not a regular file!";

			return oss.str();
		}
		
		auto fileStatus = status(target);
		auto filePerms = fileStatus.permissions();
		
		bool canRead = (filePerms & (
			perms::owner_read
			| perms::group_read
			| perms::others_read))  
			!= perms::none;
		
		if (!canRead)
		{
			oss << "Failed to read binary lines from target '" << target << "' because of insufficient read permissions!";

			return oss.str();
		}

		try
		{
			ifstream in(
				target,
				ios::in
				| ios::binary);

			if (in.fail()
				&& errno != 0)
			{
				int err = errno;

				oss << "Failed to read binary lines from target '" << target
					<< "' because it couldn't be opened! "
					<< "Reason: (errno " << err << "): " << strerror(err);

				return oss.str();
			}

			in.seekg(0, ios::end);
			size_t fileSize = scast<size_t>(in.tellg());

			if (fileSize == 0)
			{
				in.close();

				oss << "Failed to read binary lines from target '" << target << "' because it had no data!";

				return oss.str();
			}

			if (rangeEnd == 0) rangeEnd = fileSize;

			if (rangeEnd <= rangeStart)
			{
				in.close();

				oss << "Failed to read binary lines from target '" << target << "' because rangeEnd '"
					<< rangeEnd << "' is lower or equal to rangeStart '" << rangeStart << "'!";

				return oss.str();
			}
			if (rangeStart >= rangeEnd)
			{
				in.close();

				oss << "Failed to read binary lines from target '" << target << "' because rangeStart '"
					<< rangeStart << "' is higher or equal to rangeEnd '" << rangeEnd << "'!";

				return oss.str();
			}
			if (rangeStart >= fileSize)
			{
				in.close();

				oss << "Failed to read binary lines from target '" << target << "' because rangeStart '"
					<< rangeStart << "' is higher or equal to file size '" << fileSize << "'!";

				return oss.str();
			}
			if (rangeEnd > fileSize)
			{
				in.close();

				oss << "Failed to read binary lines from target '" << target << "' because rangeEnd '"
					<< rangeEnd << "' is higher than file size '" << fileSize << "'!";

				return oss.str();
			}

			size_t readSize = rangeEnd - rangeStart;
			allData.resize(readSize);

			in.seekg(scast<streamoff>(rangeStart), ios::beg);
			in.read(
				rcast<char*>(allData.data()),
				scast<streamsize>(readSize));

			if (in.fail()
				&& errno != 0)
			{
				int err = errno;

				oss << "Failed to read binary lines from target '" << target
					<< "' because it couldn't be opened! "
					<< "Reason: (errno " << err << "): " << strerror(err);

				return oss.str();
			}

			size_t bytesRead = scast<size_t>(in.gcount());

			in.close();

			if (bytesRead != readSize)
			{
				oss << "Failed to read binary lines from target '" << target << "'!"
					<< " Expected size was '" << readSize << "' bytes but result was '" << bytesRead << "' bytes.";

				return oss.str();
			}

			//successfully got data
			outData = move(allData);
		}
		catch (exception& e)
		{
			oss << "Failed to read binary from target '" << target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}
	
	inline void WriteFixedString(
		vector<u8>& data,
		size_t offset,
		const char* str,
		size_t length)
	{
		//determine how many characters to actually write
		const size_t safeLen = min(strlen(str), length);
		
		//append data to the end of the file
		if (offset == scast<size_t>(-1))
		{
			for (size_t i = 0; i < length; i++)
			{
				if (i < safeLen)
				{
					data.push_back(scast<u8>(str[i]));
				}
				else data.push_back(0); //null-pad remaining bytes
			}
			return;
		}
		
		//write at target offset, rewrite if needed
		if (offset + length > data.size())
		{
			data.resize(offset + length);
		}
			
		for (size_t i = 0; i < length; i++)
		{
			if (i < safeLen)
			{
				data[offset + i] = scast<u8>(str[i]);
			}
			else data[offset + i] = 0; //null-pad remaining bytes
		}
	}
	inline string ReadFixedString(
		const vector<u8>& data,
		size_t offset,
		size_t length)
	{
		//return empty string if trying to read out of bounds
		if (offset + length > data.size()) return "";
		
		string result{};
		result.reserve(length);
		
		for (size_t i = 0; i < length; i++)
		{
			char c = scast<char>(data[offset + i]);
			if (c == '\0') break; //break at first null byte
			result.push_back(c);
		}
		
		return result;
	}
	
	inline void WriteU8(
		vector<u8>& data,
		size_t offset,
		u8 value)
	{
		//append data to the end of the file
		if (offset == scast<size_t>(-1))
		{
			data.push_back(value);
			return;
		}
		
		//write at target offset, auto-resize if needed
		if (offset >= data.size()) data.resize(offset + 1);
		
		data[offset] = value;
	}
	inline void WriteU16(
		vector<u8>& data,
		size_t offset,
		u16 value)
	{
		//append data to the end of the file
		if (offset == scast<size_t>(-1))
		{
			data.push_back(scast<u8>(value & 0xFF));
			data.push_back(scast<u8>((value >> 8) & 0xFF));
			return;
		}
		
		//write at target offset, auto-resize if needed
		if (offset + 1 >= data.size()) data.resize(offset + 2);
		
		data[offset]     = scast<u8>(value & 0xFF);
		data[offset + 1] = scast<u8>((value >> 8) & 0xFF);
	}
	inline void WriteU32(
		vector<u8>& data,
		size_t offset,
		u32 value)
	{
		//append data to the end of the file
		if (offset == scast<size_t>(-1))
		{
			data.push_back(scast<u8>(value & 0xFF));
			data.push_back(scast<u8>((value >> 8) & 0xFF));
			data.push_back(scast<u8>((value >> 16) & 0xFF));
			data.push_back(scast<u8>((value >> 24) & 0xFF));
			return;
		}
		
		//write at target offset, auto-resize if needed
		if (offset + 3 >= data.size()) data.resize(offset + 4);
		
		data[offset]     = scast<u8>(value & 0xFF);
		data[offset + 1] = scast<u8>((value >> 8) & 0xFF);
		data[offset + 2] = scast<u8>((value >> 16) & 0xFF);
		data[offset + 3] = scast<u8>((value >> 24) & 0xFF);
	}
	
	inline u8 ReadU8(
		const vector<u8>& data,
		size_t offset)
	{
		return (offset >= data.size())
			? 0x00
			: scast<u8>(data[offset]);
	}
	inline u16 ReadU16(
		const vector<u8>& data,
		size_t offset)
	{
		return (offset + 1 >= data.size())
			? 0x0000
			: scast<u16>(data[offset])
			| scast<u16>(data[offset + 1]) << 8;
	}
	inline u32 ReadU32(
		const vector<u8>& data,
		size_t offset)
	{
		return (offset + 3 >= data.size())
			? 0x00000000
			: scast<u32>(data[offset])
			| scast<u32>(data[offset + 1]) << 8
			| scast<u32>(data[offset + 2]) << 16
			| scast<u32>(data[offset + 3]) << 24;
	}
	
	inline void WriteI8(
		vector<u8>& data,
		size_t offset,
		i8 value)
	{
		//append data to the end of the file
		if (offset == scast<size_t>(-1))
		{
			data.push_back(scast<u8>(value));
			return;
		}
		
		//writes at target offset, auto-resizes if needed
		if (offset >= data.size()) data.resize(offset + 1);
		
		data[offset] = value;
	}
	inline void WriteI16(
		vector<u8>& data,
		size_t offset,
		i16 value)
	{
		//append data to the end of the file
		if (offset == scast<size_t>(-1))
		{
			data.push_back(scast<u8>(value & 0xFF));
			data.push_back(scast<u8>((value >> 8) & 0xFF));
			return;
		}
		
		//writes at target offset, auto-resizes if needed
		if (offset + 1 >= data.size()) data.resize(offset + 2);
		
		data[offset]     = scast<u8>(value & 0xFF);
		data[offset + 1] = scast<u8>((value >> 8) & 0xFF);
	}
	inline void WriteI32(
		vector<u8>& data,
		size_t offset,
		i32 value)
	{
		//append data to the end of the file
		if (offset == scast<size_t>(-1))
		{
			data.push_back(scast<u8>(value & 0xFF));
			data.push_back(scast<u8>((value >> 8) & 0xFF));
			data.push_back(scast<u8>((value >> 16) & 0xFF));
			data.push_back(scast<u8>((value >> 24) & 0xFF));
			return;
		}
		
		//writes at target offset, auto-resizes if needed
		if (offset + 3 >= data.size()) data.resize(offset + 4);
		
		data[offset]     = scast<u8>(value & 0xFF);
		data[offset + 1] = scast<u8>((value >> 8) & 0xFF);
		data[offset + 2] = scast<u8>((value >> 16) & 0xFF);
		data[offset + 3] = scast<u8>((value >> 24) & 0xFF);
	}
	
	inline i8 ReadI8(
		const vector<u8>& data,
		size_t offset)
	{
		return (offset >= data.size())
			? 0
			: scast<i8>(data[offset]);
	}
	inline i16 ReadI16(
		const vector<u8>& data,
		size_t offset)
	{
		if (offset + 1 >= data.size()) return 0;
		
		u16 value =
			scast<u16>(data[offset])
			| scast<u16>(data[offset + 1]) << 8;
			
		return scast<i16>(value);
	}
	inline i32 ReadI32(
		const vector<u8>& data,
		size_t offset)
	{
		if (offset + 3 >= data.size()) return 0;
		
		u32 value =
			scast<u32>(data[offset])
			| scast<u32>(data[offset + 1]) << 8
			| scast<u32>(data[offset + 2]) << 16
			| scast<u32>(data[offset + 3]) << 24;
			
		return scast<i32>(value);
	}
	
	//Return all start and end of defined string in a binary
	inline string GetRangeByValue(
		const path& target,
		const string& inData,
		vector<BinaryRange>& outData)
	{
		ostringstream oss{};

		if (!exists(target))
		{
			oss << "Failed to get binary data range from target '" << target << "' because it does not exist!";

			return oss.str();
		}
		if (!is_regular_file(target))
		{
			oss << "Failed to get binary data range from target '" << target << "' because it is not a regular file!";

			return oss.str();
		}
		
		if (inData.empty())
		{
			oss << "Failed to get binary data range from target '" << target << "' because input string was empty!";

			return oss.str();
		}
		
		auto fileStatus = status(target);
		auto filePerms = fileStatus.permissions();
		
		bool canRead = (filePerms & (
			perms::owner_read
			| perms::group_read
			| perms::others_read))  
			!= perms::none;
		
		if (!canRead)
		{
			oss << "Failed to get range by value from target '" << target << "' because of insufficient read permissions!";

			return oss.str();
		}

		try
		{
			ifstream in(
				target,
				ios::binary);

			if (in.fail()
				&& errno != 0)
			{
				int err = errno;

				oss << "Failed to get ragge by value from target '" << target
					<< "' because it couldn't be opened! "
					<< "Reason: (errno " << err << "): " << strerror(err);

				return oss.str();
			}

			size_t fileSize{};
			string result = GetFileSize(
				target,
				fileSize);

			if (!result.empty())
			{
				ostringstream oss{};
				oss << "Failed to get range by value for target '" << target
					<< "'! Reason: " << result;

				return oss.str();
			}

			if (fileSize == 0)
			{
				ostringstream oss{};
				oss << "Failed to get range by value for target '" << target
					<< "' because target file is empty!";

				return oss.str();
			}

			size_t patternSize = inData.size();
			if (patternSize == 0)
			{
				ostringstream oss{};
				oss << "Failed to get range by value for target '" << target
					<< "' because input pattern is empty!";

				return oss.str();
			}

			size_t chunkSize = GetBinaryChunkStreamSize(fileSize);

			vector<uint8_t> buffer(chunkSize + patternSize - 1);
			size_t offset{};
			bool firstRead = true;

			while (in)
			{
				size_t preserve{};

				if (!firstRead)
				{
					preserve = patternSize > 1 ? patternSize - 1 : 0;
					memmove(
						buffer.data(),
						buffer.data() + chunkSize,
						preserve);
					offset -= preserve;
				}

				in.read(rcast<char*>(buffer.data() + preserve), chunkSize);

				if (in.fail()
					&& errno != 0)
				{
					int err = errno;

					oss << "Failed to get range by value from target '" << target
						<< "' because it couldn't be opened! "
						<< "Reason: (errno " << err << "): " << strerror(err);

					return oss.str();
				}

				size_t bytesRead = scast<size_t>(in.gcount());

				if (bytesRead == 0) break;

				size_t totalBytes = bytesRead + preserve;

				auto first = buffer.begin();
				while (true)
				{
					auto it = search(
						first,
						buffer.begin() + totalBytes,
						inData.begin(),
						inData.end());

					if (it == buffer.begin() + totalBytes) break;

					size_t start = offset + scast<size_t>(distance(buffer.begin(), it));
					size_t end = start + inData.size();

					outData.push_back({ start, end });

					first = it + inData.size();
				}

				offset += bytesRead;
				firstRead = false;
			}
		}
		catch (exception& e)
		{
			oss << "Failed to get binary data range from target '" << target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}

	//Return all start and end of defined bytes in a binary
	inline string GetRangeByValue(
		const path& target,
		const vector<uint8_t>& inData,
		vector<BinaryRange>& outData)
	{
		ostringstream oss{};

		if (!exists(target))
		{
			oss << "Failed to get binary data range from target '" << target << "' because it does not exist!";

			return oss.str();
		}
		if (!is_regular_file(target))
		{
			oss << "Failed to get binary data range from target '" << target << "' because it is not a regular file!";

			return oss.str();
		}
		
		if (inData.empty())
		{
			oss << "Failed to get binary data range from target '" << target << "' because input vector was empty!";

			return oss.str();
		}
		
		auto fileStatus = status(target);
		auto filePerms = fileStatus.permissions();
		
		bool canRead = (filePerms & (
			perms::owner_read
			| perms::group_read
			| perms::others_read))  
			!= perms::none;
		
		if (!canRead)
		{
			oss << "Failed to get range by value from target '" << target << "' because of insufficient read permissions!";

			return oss.str();
		}

		try
		{
			ifstream in(
				target,
				ios::binary);

			if (in.fail()
				&& errno != 0)
			{
				int err = errno;

				oss << "Failed to get range by value from target '" << target
					<< "' because it couldn't be opened! "
					<< "Reason: (errno " << err << "): " << strerror(err);

				return oss.str();
			}

			size_t fileSize{};
			string result = GetFileSize(
				target, 
				fileSize);

			if (!result.empty())
			{
				ostringstream oss{};
				oss << "Failed to get range by value for target '" << target
					<< "'! Reason: " << result;

				return oss.str();
			}

			if (fileSize == 0)
			{
				ostringstream oss{};
				oss << "Failed to get range by value for target '" << target
					<< "' because target file is empty!";

				return oss.str();
			}

			size_t patternSize = inData.size();
			if (patternSize == 0)
			{
				ostringstream oss{};
				oss << "Failed to get range by value for target '" << target
					<< "' because input pattern is empty!";

				return oss.str();
			}

			size_t chunkSize = GetBinaryChunkStreamSize(fileSize);

			vector<uint8_t> buffer(chunkSize + patternSize - 1);
			size_t offset{};
			bool firstRead = true;

			while (in)
			{
				size_t preserve{};

				if (!firstRead)
				{
					preserve = patternSize > 1 ? patternSize - 1 : 0;
					memmove(
						buffer.data(),
						buffer.data() + chunkSize,
						preserve);
					offset -= preserve;
				}

				in.read(rcast<char*>(buffer.data() + preserve), chunkSize);

				if (in.fail()
					&& errno != 0)
				{
					int err = errno;

					oss << "Failed to get range by value from target '" << target
						<< "' because it couldn't be opened! "
						<< "Reason: (errno " << err << "): " << strerror(err);

					return oss.str();
				}

				size_t bytesRead = scast<size_t>(in.gcount());

				if (bytesRead == 0) break;

				size_t totalBytes = bytesRead + preserve;

				auto first = buffer.begin();
				while (true)
				{
					auto it = search(
						first,
						buffer.begin() + totalBytes,
						inData.begin(),
						inData.end());

					if (it == buffer.begin() + totalBytes) break;

					size_t start = offset + scast<size_t>(distance(buffer.begin(), it));
					size_t end = start + inData.size();

					outData.push_back({ start, end });

					first = it + inData.size();
				}

				offset += bytesRead;
				firstRead = false;
			}
		}
		catch (exception& e)
		{
			oss << "Failed to get binary data range from target '" << target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}
}