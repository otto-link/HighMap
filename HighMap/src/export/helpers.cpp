/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <filesystem>

namespace hmap
{

std::filesystem::path add_filename_suffix(
    const std::filesystem::path &file_path,
    const std::string           &suffix)
{

  std::filesystem::path stem = file_path.stem();
  std::filesystem::path ext = file_path.extension();
  std::filesystem::path new_path = file_path.parent_path() /
                                   std::filesystem::path(stem.string() +
                                                         suffix + ext.string());
  return new_path;
}

} // namespace hmap
