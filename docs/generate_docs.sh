# Get the external documentation
mkdir -p data_container
wget -q -nc -P ./data_container https://raw.githubusercontent.com/schombert/DataContainer/master/README.md
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
find $1 -maxdepth 1 -name "*.md" $3 -print0 |
	while IFS= read -r -d '' line; do
		read -r firstline < $line
		firstline=$(echo $firstline | cut -d ' ' -f2-)
		line=$(basename $line .md)
		line=`echo $line | sed 's/\([A-Z]\)/_\1/g' | tr '[A-Z]' '[a-z]'`
		echo '<tab type="user" visible="yes" url="'md_$2${line}.html'" title="'$firstline'"/>' \
			>> layout_file.xml
	done
}

gen_directory "" "" "! -name about.md"
echo '<tab type="usergroup" url="'md_data_container__r_e_a_d_m_e.html'" title="Data Container">' \
				>> layout_file.xml
gen_directory "data_container/" "data_container_" "! -name README.md"
echo '</tab>' 	>> layout_file.xml


echo '</navindex>'						>> layout_file.xml
echo '</doxygenlayout>' 	>> layout_file.xml

# Run doxygen
doxygen

# Replace broken code blocks in headers
sed -i "s/&lt;tt&gt;/<code>/g" out/html/md_*.html
sed -i "s/&lt;\/tt&gt;/<\/code>/g" out/html/md_*.html