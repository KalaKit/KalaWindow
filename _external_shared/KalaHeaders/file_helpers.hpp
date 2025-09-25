//------------------------------------------------------------------------------
// file_helpers.hpp
//
// Copyright (C) 2025 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
//
// Provides:
//   - file management - copy, move, delete, rename, list directory contents
//   - file metadata - file size, directory size, line count, get filename (stem + extension), get stem, get parent, get/set extension
//   - text I/O - read/write data for text files with vector of string lines or string blob
//   - binary I/O - read/write data for binary files with vector of bytes or buffer + size
//------------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <filesystem>

namespace KalaHeaders
{
	using std::exception;
	using std::string;
	using std::vector;
	using std::ostringstream;
	using std::ifstream;
	using std::ofstream;
	using std::streamsize;
	using std::streamoff;
	using std::ios;
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
	using std::filesystem::file_size;
	using std::filesystem::recursive_directory_iterator;
	using std::filesystem::directory_iterator;

	//
	// FILE MANAGEMENT
	//

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

	//
	// FILE METADATA
	//

	//Get the size of the target file in bytes
	inline string GetFileSize(
		const path& target,
		uintmax_t& outSize)
	{
		ostringstream oss{};
		size_t totalSize{};

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

		try
		{
			ifstream in(target, ios::in);

			if (!in.is_open())
			{
				oss << "Failed to get target '" << target << "' line count because it couldn't be opened!";

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

	//Get the filename of the target (with extension)
	inline string GetPathName(
		const path& target,
		string& outName)
	{
		ostringstream oss{};

		if (!exists(target))
		{
			oss << "Failed to get target '"
				<< target << "' name because it does not exist!";

			return oss.str();
		}

		outName = target.filename().string();

		return{};
	}
	//Get the stem (filename without extension) of the target
	inline string GetPathStem(
		const path& target,
		string& outStem)
	{
		ostringstream oss{};

		if (!exists(target))
		{
			oss << "Failed to get target '"
				<< target << "' stem because it does not exist!";

			return oss.str();
		}

		outStem = target.stem().string();

		return{};
	}

	//Get the parent directory of the target
	inline string GetPathParent(
		const path& target,
		string& outParent)
	{
		ostringstream oss{};

		if (!exists(target))
		{
			oss << "Failed to get target '"
				<< target << "' parent path because it does not exist!";

			return oss.str();
		}
		if (!target.has_parent_path())
		{
			oss << "Failed to get parent path for target '"
				<< target << "' because it does not have a parent!";

			return oss.str();
		}

		outParent = target.parent_path().string();

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
	//Get the extension of the target
	inline string GetPathExtension(
		const path& target,
		string& outExtension)
	{
		ostringstream oss{};

		if (!exists(target))
		{
			oss << "Failed to get target '"
				<< target << "' extension because it does not exist!";

			return oss.str();
		}
		if (!is_regular_file(target))
		{
			oss << "Failed to get extension for target '"
				<< target << "' because it is not a regular file!";

			return oss.str();
		}

		outExtension = target.extension().string();

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
		bool append = false)
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

			if (!out.is_open())
			{
				oss << "Failed to write text to target '" << target << "' because it couldn't be opened!";

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

		try
		{
			ifstream in(
				target,
				ios::in);

			if (!in.is_open())
			{
				oss << "Failed to read text from target '" << target << "' because it couldn't be opened!";

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
		bool append = false)
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

			if (!out.is_open())
			{
				oss << "Failed to write lines to target '" << target << "' because it couldn't be opened!";

				return oss.str();
			}

			for (size_t i = 0; i < inLines.size(); ++i)
			{
				out << inLines[i];
				if (i + 1 < inLines.size()) out << '\n';
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

		try
		{
			ifstream in(
				target,
				ios::in);

			if (!in.is_open())
			{
				oss << "Failed to read lines from target '" << target << "' because it couldn't be opened!";

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

			if (lineEnd == 0) lineEnd == totalLines;

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

	//Write raw binary buffer (pointer + size) to a file, with optional append flag.
	//A new file is created at target path if it doesn't already exist
	inline string WriteBinaryBufferToFile(
		const path& target,
		const uint8_t* inBuffer,
		size_t bufferSize,
		bool append = false)
	{
		ostringstream oss{};

		if (exists(target)
			&& !is_regular_file(target))
		{
			oss << "Failed to write binary buffer to target '" << target << "' because it is not a regular file!";

			return oss.str();
		}
		if (!inBuffer)
		{
			oss << "Failed to write binary buffer to target '" << target << "' because inBuffer was null!";

			return oss.str();
		}
		if (bufferSize == 0)
		{
			oss << "Failed to write binary buffer to target '" << target << "' because bufferSize was 0!";

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

			if (!out.is_open())
			{
				oss << "Failed to write binary buffer to target '" << target << "' because it couldn't be opened!";

				return oss.str();
			}

			out.write(
				reinterpret_cast<const char*>(inBuffer),
				static_cast<streamsize>(bufferSize));

			out.close();
		}
		catch (exception& e)
		{
			oss << "Failed to write binary buffer to target '" << target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}
	//Read raw binary data from a file into a buffer (up to bufferSize bytes).
	//OutBytesRead returns the number of bytes read
	inline string ReadBinaryBufferFromFile(
		const path& target,
		uint8_t* outBuffer,
		size_t bufferSize,
		size_t& outBytesRead)
	{
		ostringstream oss{};
		outBytesRead = 0;

		if (!exists(target))
		{
			oss << "Failed to read binary buffer from target '" << target << "' because it does not exist!";

			return oss.str();
		}
		if (!is_regular_file(target))
		{
			oss << "Failed to read binary buffer from target '" << target << "' because it is not a regular file!";

			return oss.str();
		}
		if (!outBuffer)
		{
			oss << "Failed to read binary buffer from target '" << target << "' because outBuffer was null!";

			return oss.str();
		}
		if (bufferSize == 0)
		{
			oss << "Failed to read binary buffer from target '" << target << "' because bufferSize was 0!";

			return oss.str();
		}

		try
		{
			ifstream in(
				target,
				ios::in
				| ios::binary);

			if (!in.is_open())
			{
				oss << "Failed to read binary buffer from target '" << target << "' because it couldn't be opened!";

				return oss.str();
			}

			in.read(
				reinterpret_cast<char*>(outBuffer),
				static_cast<streamsize>(bufferSize));

			outBytesRead = static_cast<size_t>(in.gcount());

			in.close();

			if (outBytesRead == 0)
			{
				oss << "Failed to read binary buffer from target '" << target << "' because no data was read!";

				return oss.str();
			}
		}
		catch (exception& e)
		{
			oss << "Failed to read binary buffer from target '" << target << "'! Reason: " << e.what();

			return oss.str();
		}

		return{};
	}

	//Write all binary data from a vector<uint8_t> to a file, with optional append flag.
	//A new file is created at target path if it doesn't already exist
	inline string WriteBinaryLinesToFile(
		const path& target,
		const vector<uint8_t>& inData,
		bool append = false)
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

			if (!out.is_open())
			{
				oss << "Failed to write binary to target '" << target << "' because it couldn't be opened!";

				return oss.str();
			}

			out.write(
				reinterpret_cast<const char*>(inData.data()),
				static_cast<streamsize>(inData.size()));

			out.close();
		}
		catch (exception& e)
		{
			oss << "Failed to write binary to target '" << target << "'! Reason: " << e.what();

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

		try
		{
			ifstream in(
				target,
				ios::in
				| ios::binary);

			if (!in.is_open())
			{
				oss << "Failed to read binary from target '" << target << "' because it couldn't be opened!";

				return oss.str();
			}

			in.seekg(0, ios::end);
			size_t fileSize = static_cast<size_t>(in.tellg());

			if (fileSize == 0)
			{
				in.close();

				oss << "Failed to read binary from target '" << target << "' because it had no data!";

				return oss.str();
			}

			if (rangeEnd == 0) rangeEnd = fileSize;

			if (rangeEnd <= rangeStart)
			{
				in.close();

				oss << "Failed to read binary from target '" << target << "' because rangeEnd '"
					<< rangeEnd << "' is lower or equal to rangeStart '" << rangeStart << "'!";

				return oss.str();
			}
			if (rangeStart >= rangeEnd)
			{
				in.close();

				oss << "Failed to read binary from target '" << target << "' because rangeStart '"
					<< rangeStart << "' is higher or equal to rangeEnd '" << rangeEnd << "'!";

				return oss.str();
			}
			if (rangeStart >= fileSize)
			{
				in.close();

				oss << "Failed to read binary from target '" << target << "' because rangeStart '"
					<< rangeStart << "' is higher or equal to file size '" << fileSize << "'!";

				return oss.str();
			}
			if (rangeEnd > fileSize)
			{
				in.close();

				oss << "Failed to read binary from target '" << target << "' because rangeEnd '"
					<< rangeEnd << "' is higher than file size '" << fileSize << "'!";

				return oss.str();
			}

			size_t readSize = rangeEnd - rangeStart;
			allData.resize(readSize);

			in.seekg(static_cast<streamoff>(rangeStart), ios::beg);
			in.read(
				reinterpret_cast<char*>(allData.data()),
				static_cast<streamsize>(readSize));

			size_t bytesRead = static_cast<size_t>(in.gcount());

			in.close();

			if (bytesRead != readSize)
			{
				oss << "Failed to read binary from target '" << target << "'!"
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
}