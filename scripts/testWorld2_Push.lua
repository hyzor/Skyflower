
hej = ""

function load(id)
	Print("AI loaded")
	hej = "hej där"
	
	StartUpdate()
end

function update(id, dt)
	Print(hej)
	
	if not IsActivated(55) then
		SetTarget(55) --start button
	else
		SetTarget(1) --hunt player
	end
end

function activated(id)
	--Print("Player pushed")
end

function deactivated(id)
	
end