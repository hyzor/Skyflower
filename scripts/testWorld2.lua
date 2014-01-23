
trapButtonDown = false



--aiPush script--
-----------------

function load_aiPush(id)
	Print("AI loaded")
	
	StartUpdate()
end

function update_aiPush(id, dt)
		
	if not trapButtonDown then
		SetTarget(id, 55) --start button
		if IsTouching(id, 55) then
			Jump(id)
		end
	else
		SetTarget(id, 1) --hunt player
		if CanPush(id, 1) then
			Push(id, 1)
		end
	end
end

function activated_aiPush(id)
	--Print("Player pushed")
end

function deactivated_aiPush(id)
	
end




--btnTrap script--
------------------

function activated_btnTrap(id)

	--trap!
	--Spawn(36, 9) --spawn enemy(10) on spawnpoint(7)
	Spawn(37, 9) --spawn enemy(11) on spawnpoint(7)
	
	trapButtonDown = true
end

function deactivated_btnTrap(id)
	--trapButtonDown = false
end



--btnBig script--
-----------------

function activated_btnBig(id)
	--PlaySound(id, "player/jump1.wav")
	Jump(3) --woodblock
end







function activated_btnPlatformMove(id)
	ToggleOscillatePosition(99)
end


function deactivated_btnPlatformMove(id)
	ToggleOscillatePosition(99)
end





function activated_btnLevelCompleted(id)
	
end

function deactivated_btnLevelCompleted(id)
	if Level() ~= 0 then
		ChangeLevel(0) --hubb värld
	else
		ChangeLevel(1)
	end
end









