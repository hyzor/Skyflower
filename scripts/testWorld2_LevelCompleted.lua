
function activated(id)
	
end

function deactivated(id)
	if Level() ~= 0 then
		ChangeLevel(0) --hubb värld
	else
		ChangeLevel(1)
	end
end