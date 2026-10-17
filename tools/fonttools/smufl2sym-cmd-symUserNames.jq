to_entries[] | [
	"    ",
	if (.key | in($tr[0])) then "kfw::TranslatableString(\"engraving/sym\", \"" else "kfw::TranslatableString::untranslatable(\"" end,
	(.value.description | gsub("\""; "\\\"")),
	if (.key | startswith("accidentalWyschnegradsky")) then " (Wyschnegradsky)" else "" end,
	"\"),"
] | join("")
