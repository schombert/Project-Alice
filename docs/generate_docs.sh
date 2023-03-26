# Get the external documentation
mkdir -p data_container
wget -q -nc -O ./data_container/data_container.md https://raw.githubusercontent.com/schombert/DataContainer/master/README.md
wget -q -nc -P ./data_container https://raw.githubusercontent.com/schombert/DataContainer/master/changes.md
wget -q -nc -P ./data_container https://raw.githubusercontent.com/schombert/DataContainer/master/file_format_documentation.md
wget -q -nc -P ./data_container https://raw.githubusercontent.com/schombert/DataContainer/master/getting_started.md
wget -q -nc -P ./data_container https://raw.githubusercontent.com/schombert/DataContainer/master/iamtheecs.jpg
wget -q -nc -P ./data_container https://raw.githubusercontent.com/schombert/DataContainer/master/iamtheecs.jpg
wget -q -nc -P ./data_container https://raw.githubusercontent.com/schombert/DataContainer/master/multithreading.md
wget -q -nc -P ./data_container https://raw.githubusercontent.com/schombert/DataContainer/master/objects_and_properties.md
wget -q -nc -P ./data_container https://raw.githubusercontent.com/schombert/DataContainer/master/overview.md
wget -q -nc -P ./data_container https://raw.githubusercontent.com/schombert/DataContainer/master/queries.md
wget -q -nc -P ./data_container https://raw.githubusercontent.com/schombert/DataContainer/master/relationships.md
wget -q -nc -P ./data_container https://raw.githubusercontent.com/schombert/DataContainer/master/serialization.md
wget -q -nc -P ./data_container https://raw.githubusercontent.com/schombert/DataContainer/master/tutorial_color.md
wget -q -nc -P ./data_container https://raw.githubusercontent.com/schombert/DataContainer/master/tutorial_ecs.md
wget -q -nc -P ./data_container https://raw.githubusercontent.com/schombert/DataContainer/master/ve_documentation.md

# Generate the layout file
echo '<doxygenlayout version="1.0">' 	> layout_file.xml
echo '<navindex>'						>> layout_file.xml

gen_directory () {
directory=$1
exclude_file=$2
# The url needs to replace './' with '_' and remove the first .
url_directory=${directory:1}
url_directory="${url_directory//\//_}"

# Recursively generate for sub directories
find $directory -maxdepth 1 -type d -print0 |
	while IFS= read -r -d '' path; do
		# Exlcude the current directory and the out directory
		if [ $path == $directory ] || [ $path == './out' ]; then
			continue
		fi

		new_directory=$(basename $path)
		if find $path -type f -name "${new_directory}.md" -print -quit | grep -q .; then
			# Set the title to the first line excluding the ## header
			read -r firstline < "${path}/${new_directory}.md"
			title=$(echo $firstline | cut -d ' ' -f2-)

			# The url needs to replace './' with '_' and remove the first .
			file_name="${path:1}_${new_directory}"
			file_name="${file_name//\//_}"
			file_name=`echo $file_name | sed 's/\([A-Z]\)/_\1/g' | tr '[A-Z]' '[a-z]'`
			echo ${file_name}
		 	# If there is a file with the same name as the folder use that as the tab
			echo '<tab type="usergroup" url="'md${file_name}.html'" title="'${title}'">' \
					>> layout_file.xml
			gen_directory ${path} "! -name ${new_directory}.md"
		else
		 	# If there isn't we just use a regular tab
			echo '<tab type="usergroup" title="'${new_directory}'">' \
							>> layout_file.xml
			gen_directory ${path} ""
		fi
		echo '</tab>' 	>> layout_file.xml
	done

# For each .md file in the directory
find $directory -maxdepth 1 -type f -name "*.md" ${exclude_file} -print0 |
	while IFS= read -r -d '' file; do
		# Set the title to the first line excluding the ## header
		read -r firstline < $file
		title=$(echo $firstline | cut -d ' ' -f2-)
		# Get the file name
		file_name=$(basename $file .md)
		# Replace the uppercase characters to _+lowercase, ex: ABC -> _a_b_c
		# This is cause that's how doxygen formats the url
		file_name=`echo $file_name | sed 's/\([A-Z]\)/_\1/g' | tr '[A-Z]' '[a-z]'`
		# Output the markdown file to the layout file
		echo '<tab type="user" visible="yes" url="'md${url_directory}_${file_name}.html'" title="'$title'"/>' \
			>> layout_file.xml
	done
}

gen_directory "." "! -name about.md"

echo '</navindex>'						>> layout_file.xml
echo '</doxygenlayout>' 	>> layout_file.xml

# Copy all the pngs recursivly to the html folder
find . -type f -not -path "./out/*" -name "*.png" -exec cp {} ./out/html/ \;
